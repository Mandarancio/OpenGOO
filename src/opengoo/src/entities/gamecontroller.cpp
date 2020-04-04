#include <QtMath>

#include "GameEngine/og_gameengine.h"
#include "GameEngine/scene.h"
#include "GameEngine/input.h"

#include "GameConfiguration/wog_ball.h"

#include "PhysicsEngine/circleshape.h"

#include "og_sprite.h"
#include "gamecontroller.h"
#include "ball.h"
#include "entityfactory.h"
#include "rotationanimation.h"
#include "strand.h"

inline OGSpritePtr CreateSprite(const QString& aId)
{
    auto src = GE->GetResourceManager()->GetImageSourceById(aId);
    return OGSprite::Create(src);
}

class GameController::Exit
{
public:
    static const int SuckSpeed = 8;

    class CircleSensor
    {
    public:
        typedef og::physics::Shape Shape;
        typedef og::physics::CircleShape CircleShape;

    public:
        CircleSensor(float aRadius, float aX, float aY)
            : mPosition(aX, aY)
            , mRadius(aRadius)
        {
        }

        bool IsTouching(const Shape& aShape, const QVector2D aPosition, float aRadius) const
        {
            b2CircleShape shape;
            shape.m_radius = mRadius;
            shape.m_p.Set(mPosition.x(), mPosition.y());
            CircleShape s(&shape);
            Shape::Transform tA;
            tA.angle = 0;
            Shape::Transform tB;
            tB.position = aPosition;
            tB.angle = aRadius;
            return Shape::Collide(s, tA, aShape, tB);
        }

        const QVector2D& GetPosition() const
        {
            return mPosition;
        }

    private:
        QVector2D mPosition;
        float mRadius;
    };

public:
    Exit(float aX, float aY, float aSuckRadius, std::vector<std::shared_ptr<Ball>>& aBalls, float aRatio)
        : mPosition(aX, aY)
        , mSensor(aSuckRadius * aRatio, aX * aRatio, -aY * aRatio)
        , mBalls(aBalls)
        , mSuckedBalls(0)
    {
        mSpeed = (float)SuckSpeed / GE->getFrameRate();
        mExitRadius = SuckSpeed * aRatio;
    }

    static bool IsSuckable(const Ball* aBall)
    {
        return (aBall->IsSuckable() && aBall->GetVisible() && !(aBall->IsSleeping() || aBall->IsMarker() || aBall->IsAttached()));
    }

    void Update()
    {
        for (auto it = mBalls.begin(); it != mBalls.end(); ++it)
        {
            if (!IsSuckable((*it).get()))
            {
                continue;
            }

            if (mSensor.IsTouching((*it)->GetShape(), (*it)->GetPhyPosition(), 0))
            {
                if ((*it)->IsClimbing())
                {
                    (*it)->OnSuction();
                }

                auto v = mSensor.GetPosition() - (*it)->GetPhyPosition();
                if (v.length() < mExitRadius)
                {
                    (*it)->OnExit();
                    ++mSuckedBalls;
                }
                else
                {
                    v.normalize();
                    (*it)->PhyMoveBy(v * mSpeed);
                }
            }
        }
    }

    const QVector2D& GetPosition() const
    {
        return mPosition;
    }

    int GetSuckedBalls() const
    {
        return mSuckedBalls;
    }

private:
    QVector2D mPosition;
    CircleSensor mSensor;
    std::vector<std::shared_ptr<Ball>>& mBalls;
    float mSpeed;
    float mExitRadius;
    int mSuckedBalls;
};

GameController::GameController()
    : Entity(0, 0)
    , mMusic(nullptr)
    , mMarker(nullptr)
    , mBallDef(nullptr)
    , mState(e_finding_marker)
    , mNearestBall(nullptr)
    , mShouldSuckingBalls(false)
    , mBallsRequired(0)
    , mIsWon(false)
{
}

void GameController::Deleter::operator()(Exit* aPtr) const
{
    delete aPtr;
}

void GameController::Added()
{
    if (mMusic)
    {
        mMusic->Play(true);
    }

    foreach (auto& snd, mLoopSounds)
    {
        snd->Play();
    }
}

void GameController::Removed()
{
}

void GameController::FillNearsAttachedBalls(int aNum, int aLen, const QVector2D& aPos)
{
    mNearestAttachedBalls.clear();
    if (aNum == 0)
    {
        return;
    }

    auto begin = mAttachedBall.begin();
    auto lengthSquared = aLen * aLen;
    for (int i = 0; i< aNum; ++i)
    {
        auto b = std::find_if(begin, mAttachedBall.end(), [lengthSquared, &aPos](std::shared_ptr<Ball>& b)
        {
            auto v = b->GetPosition() - aPos;
            return v.lengthSquared() <= lengthSquared;
        });

        if (b == mAttachedBall.end())
        {
            return;
        }

        mNearestAttachedBalls.push_back(*b);
        begin = ++b;
    }
}

void GameController::ResetMarker()
{
    mMarker.reset();
    mBallDef = nullptr;
    mNearestAttachedBalls.clear();
}

void GameController::MarkBall(std::shared_ptr<Ball>& aBall)
{
    if (!aBall->IsDraggable()) {
        return;
    }

    mMarker = aBall;
    mBallDef = GE->GetResourceManager()->GetBallByType(mMarker->GetName());
    if (!mBallDef)
    {
        return;
    }

    auto src = GE->GetResourceManager()->GetImageSourceById(mBallDef->marker.drag);
    auto spr = OGSprite::Create(src);
    spr->CenterOrigin();

    float angleSpeed = -qRadiansToDegrees(mBallDef->marker.rotspeed) / 1000;
    auto anim = std::make_shared<RotationAnimation>(angleSpeed);
    spr->SetAnimation(anim)->Start();
    mMarker->Mark(spr);

    mState = e_marker_is_found;
}

void GameController::UnMarkBall()
{
    mMarker->UnMark();
    ResetMarker();
    mState = e_finding_marker;
}

void GameController::AttachMarker()
{
    CreateStrands();
    mMarker->OnAttach();
    mAttachedBall.push_back(mMarker);
    ResetMarker();
}

void GameController::CreateStrands()
{
    EntityFactory ef;
    ef.SetPhysicsEngine(GetScene()->GetPhysicsEngine());

    for (auto it = mNearestAttachedBalls.begin(); it != mNearestAttachedBalls.end(); ++it)
    {
        if (auto entity = ef.CreateStrand(mMarker, (*it).get(), *mBallDef->strand))
        {
            mMarker->AddStrand(entity.get());
            (*it).get()->AddStrand(entity.get());
            GetScene()->AddEntity(entity);
        }
    }
}

EntityPtr GameController::CreateContinueButton()
{
    EntityFactory ef;
    ef.SetPhysicsEngine(GetScene()->GetPhysicsEngine());
    return ef.CreateContinueButton();
}

std::vector<std::shared_ptr<Ball>>::iterator GameController::FindMarker(const QVector2D& aPosition)
{
    return std::find_if(mBall.begin(), mBall.end(), [&aPosition](std::shared_ptr<Ball>& ball)
    {
        return (!ball->IsAttached() && !ball->IsSleeping() && ball->GetCollider()->OverlapPoint(aPosition));
    });
}

void GameController::Update()
{
    switch (mState)
    {
    case e_finding_marker:
        {
            QVector2D pos(og::MouseInput::GetWorldPosition());
            auto it = FindMarker(pos);
            if (it != mBall.end())
            {
                MarkBall(*it);
            }
        }
        break;
    case e_marker_is_found:
        if (og::MouseInput::IsButtonPressed(og::MouseInput::e_left))
        {
            if (mMarker->IsDraggable())
            {
                mMarker->OnPickUp();
                mState = e_marker_is_dragging;
            }
        }
        else
        {
            QVector2D mousePosition(og::MouseInput::GetWorldPosition());
            if (!mMarker->GetCollider()->OverlapPoint(mousePosition))
            {
                UnMarkBall();
            }
            else
            {
                if (!mMarker->GetPhysicsBody()->IsAwake())
                {
                    mMarker->GetPhysicsBody()->SetAwake(true);
                }
            }
        }
        break;
    case e_marker_is_dragging:
        if (!og::MouseInput::IsButtonPressed(og::MouseInput::e_left))
        {
            if (!mNearestAttachedBalls.empty())
            {
                AttachMarker();
            }
            else
            {
                mMarker->Drop();
                UnMarkBall();
            }

            mState = e_finding_marker;
        }
        else
        {
            QVector2D pos(og::MouseInput::GetWorldPosition());
            pos *= GetScene()->GetPhysicsEngine()->GetRatio();
            mMarker->GetPhysicsBody()->SetPosition(pos);
            FillNearsAttachedBalls(mBallDef->attribute.core.strands, mBallDef->strand->maxlen2, mMarker->GetPosition());
            if (mBallDef->attribute.core.strands > 1 && mNearestAttachedBalls.size() < 2)
            {
                mNearestAttachedBalls.clear();
            }
        }
        break;
    }

    {
        Ball* nearestBall = nullptr;
        float minLength = 0;
        for(auto it = mAttachedBall.begin(); it != mAttachedBall.end(); ++it)
        {
            auto len = (mExitPosition - (*it)->GetPosition()).length();
            if (!nearestBall)
            {
                nearestBall = it->get();
                minLength= len;
            }
            else if (len < minLength)
            {
                minLength = len;
                nearestBall = it->get();
            }
        }

        if (nearestBall != mNearestBall)
        {
            if (mNearestBall)
            {
                mNearestBall->SetIsNearest(false);
            }

            mNearestBall = nearestBall;

            if (mNearestBall)
            {
                mNearestBall->SetIsNearest(true);
            }
        }
    }

    if (mShouldSuckingBalls)
    {
        mExit->Update();
        if (!mIsWon && mExit->GetSuckedBalls() >= mBallsRequired)
        {
            mIsWon = true;
            auto btn = CreateContinueButton();
            GetScene()->AddEntity(btn);
        }
    }
}

void GameController::Render(QPainter& a_painter)
{
    if (!mNearestAttachedBalls.empty())
    {
        a_painter.save();
        a_painter.setPen(Qt::red);

        for (auto it = mNearestAttachedBalls.begin(); it != mNearestAttachedBalls.end(); ++it)
        {
            a_painter.drawLine(mMarker->GetPosition().toPointF(), (*it).get()->GetPosition().toPointF());
        }

        a_painter.restore();
    }

    if (mExit)
    {
        a_painter.save();
        a_painter.resetTransform();
        auto fnt = a_painter.font();
        fnt.setPointSize(18);
        a_painter.setFont(fnt);
        a_painter.drawText(10, 20, QString::number(mExit->GetSuckedBalls()));
        a_painter.restore();
    }
}

void GameController::AddBall(std::shared_ptr<Ball> aBall)
{
    mBall.push_back(aBall);
    if (aBall->IsAttached())
    {
        mAttachedBall.push_back(aBall);
    }
}

void GameController::InitExit(const QPointF &aPosition, float aRadius, float aRatio)
{
    mExit.reset(new Exit(aPosition.x(), -aPosition.y(), aRadius, mBall, aRatio));
}
