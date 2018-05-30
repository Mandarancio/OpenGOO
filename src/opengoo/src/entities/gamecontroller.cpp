#include <QtMath>

#include "GameEngine/og_gameengine.h"
#include "GameEngine/scene.h"
#include "GameEngine/input.h"

#include "GameConfiguration/wog_ball.h"

#include "og_sprite.h"
#include "gamecontroller.h"
#include "ball.h"
#include "entityfactory.h"
#include "rotationanimation.h"
#include "strand.h"

GameController::GameController()
    : Entity(0, 0)
    , mMusic(nullptr)
    , mMarker(nullptr)
    , mBallDef(nullptr)
    , mState(e_finding_marker)
    , mNearestBall(nullptr)
{
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

void GameController::FillNearsBalls(int aNum, int aLen, const QVector2D& aPos)
{
    mNearestBalls.clear();
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

        mNearestBalls.push_back(*b);
        begin = ++b;
    }
}

void GameController::ResetMarker()
{
    mMarker.reset();
    mBallDef = nullptr;
    mNearestBalls.clear();
}

void GameController::MarkBall(std::shared_ptr<Ball>& aBall)
{
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
    ResetMarker();
}

void GameController::CreateStrands()
{
    EntityFactory ef;
    ef.SetPhysicsEngine(GetScene()->GetPhysicsEngine());

    mMarker->SetAttached(true);
    mAttachedBall.push_back(mMarker);
    for (auto it = mNearestBalls.begin(); it != mNearestBalls.end(); ++it)
    {
        if (auto entity = ef.CreateStrand(mMarker, (*it).get(), *mBallDef->strand))
        {
            mMarker->AddStrand(entity.get());
            (*it).get()->AddStrand(entity.get());
            GetScene()->AddEntity(entity);
        }
    }
}

std::list<std::shared_ptr<Ball>>::iterator GameController::FindMarker(const QVector2D& aPosition)
{
    return std::find_if(mBall.begin(), mBall.end(), [&aPosition](std::shared_ptr<Ball>& ball)
    {
        if (!ball->IsAttached() && !ball->IsSleeping() && ball->GetCollider()->OverlapPoint(aPosition))
        {
            return true;
        }

        return false;
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
            mMarker->OnPickUp();
            mState = e_marker_is_dragging;
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
            if (mNearestBalls.size() > 1)
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
            FillNearsBalls(mBallDef->attribute.core.strands, mBallDef->strand->maxlen2, mMarker->GetPosition());
        }
        break;
    }

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

void GameController::Render(QPainter& a_painter)
{
    if (mNearestBalls.size() > 1)
    {
        a_painter.save();
        a_painter.setPen(Qt::red);

        for (auto it = mNearestBalls.begin(); it != mNearestBalls.end(); ++it)
        {
            a_painter.drawLine(mMarker->GetPosition().toPointF(), (*it).get()->GetPosition().toPointF());
        }

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
