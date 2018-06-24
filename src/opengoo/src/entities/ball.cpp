#include <stack>
#include <unordered_set>

#include "GameEngine/input.h"
#include "GameEngine/og_gameengine.h"

#include "flags.h"

#include "scene.h"
#include "ball.h"
#include "geom.h"
#include "strand.h"

#include "balllistener.h"

#include "PhysicsEngine/polygonshape.h"
#include "PhysicsEngine/edgeshape.h"

#include <QtMath>

#include "OGLib/util.h"

bool PhysicsCollider::OverlapPoint(const QVector2D& a_point) const
{
    auto pb = GetEntity().GetPhysicsBody();
    og::physics::Shape::Transform t = {pb->GetPosition(), 0};
    return pb->GetShape().TestPoint(t, a_point * GetEntity().GetScene()->GetPhysicsEngine()->GetRatio());
}

struct BallNode
{
    Ball* data;
    BallNode* parent;

    BallNode(Ball* aBall, BallNode* aParent=nullptr)
        : data(aBall)
        , parent(aParent)
    {
    }
};

template<typename T, size_t Size=2048>
class stack_allocator
{
public:
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T value_type;

    template <class Type> struct rebind
    {
        typedef stack_allocator<Type> other;
    };

    stack_allocator() noexcept
    {
        mIndex = 0;
    }

    T* allocate (std::size_t)
    {
        assert(mIndex + sizeof(T) < Size);
        auto p = mStack + mIndex;
        mIndex += sizeof(T);
        return (T*)p;
    }

    void deallocate (T*, std::size_t)
    {
        mIndex -= sizeof(T);
    }

    template <class U>
    void destroy (U*)
    {
    }

private:
    char mStack[sizeof(T) * Size];
    size_t mIndex;
};

class DFSPathFinder : public BallPathFinder
{
    Ball* Start()
    {
        return Next();
    }

    Ball* Next()
    {
        if (mPath.empty())
        {
            return nullptr;
        }

        auto ball = mPath.back();
        mPath.pop_back();
        return ball;
    }

    BallNode* Create(Ball* aBall)
    {
        mNodes.emplace_back(aBall);
        return &mNodes.back();
    }

    void Randomize(Ball** aBalls, size_t aSize, BallNode* aNode, std::stack<BallNode*>* aOutStack)
    {
        std::random_shuffle(aBalls, aBalls + aSize);
        for (size_t i = 0; i < aSize; ++i)
        {
            auto node = Create(aBalls[i]);
            node->parent = aNode;
            aOutStack->push(node);
        }
    }

    void FindPath(Ball* aStart)
    {
        std::stack<BallNode*> stack;
        std::unordered_set<Ball*> discovered;
        BallNode* tail = nullptr;
        size_t depth = 0;

        stack.push(Create(aStart));
        while (!stack.empty())
        {
            auto v = stack.top();
            stack.pop();
            if (discovered.count(v->data) == 0)
            {
                tail = v;
                discovered.insert(v->data);
                if ((ShoulCheckNearest && v->data->IsNearest()))
                {
                    break;
                }
                if (Depth != 0 && depth == Depth)
                {
                    break;
                }
                if (size_t size = v->data->StrandCount())
                {
                    Ball* balls[size];
                    for (size_t i = 0; i < size ; ++i)
                    {
                        auto strand = v->data->GetStrand(i);
                        auto ball = strand->GetBallA();
                        if (ball == v->data)
                        {
                            ball = strand->GetBallB();
                        }

                        balls[i] = ball;
                    }

                    Randomize(balls, size, v, &stack);
                }

                ++depth;
            }
        }

        while (tail)
        {
            mPath.push_back(tail->data);
            tail = tail->parent;
        }
    }

    void Init(Ball* aStart)
    {
        mNodes.clear();
        mPath.clear();
        FindPath(aStart);
    }

private:
    std::list<BallNode, stack_allocator<BallNode>> mNodes;
    std::list<Ball*> mPath;
};

struct Queue
{
    BallNode* Pop()
    {
        auto node = mData.front();
        mData.pop();
        return node;
    }

    void Push(BallNode* aNode)
    {
        mData.push(aNode);
    }

    bool IsEmpty() const
    {
        return mData.empty();
    }

private:
    std::queue<BallNode*> mData;
};

Ball::Ball(std::unique_ptr<og::PhysicsBody> aBody, GraphicPtr aGraphic, const BallDefination& aDef)
    : Entity(0, 0, aGraphic)
    , mStartWalkSpeed(aDef.walkSpeed)
    , mStartClimbSpeed(aDef.climbSpeed)
    , mWalkSpeed(0)
    , mClimbSpeed(0)
    , mIsSleeping(aDef.isSleeping)
    , mIsAttached(false)
    , mIsDragging(false)
    , mIsMarked(false)
    , mIsStanding(false)
    , mIsSuckable(aDef.isSuckable)
    , mIsClimbing(false)
    , mIsDraggable(aDef.isDraggable)
    , mListener(nullptr)
    , mCollideWith(nullptr)
    , mIsNearest(false)
    , mPathFinder(new DFSPathFinder)
    , mDelay(0)
    , mWalkDirection(true)
    , mPassedPath(0)
{
    SetPhysicsBody(std::move(aBody));
    SetCollider(std::make_shared<PhysicsCollider>());
}

void Ball::ReverseWalk()
{
    SetWalkDirection(!GetWalkDirection());
}

void Ball::OnAttach()
{
    GetPhysicsBody()->SetActive(true);
    mIsDragging = false;
    mIsMarked = false;
    mMarkerGraphic.reset();
    SetAttached(true);
}

void Ball::OnPickUp()
{
    SetWalkSpeed(0);
    SetClimbSpeed(0);
    GetPhysicsBody()->SetActive(false);
    mIsDragging = true;
}

void Ball::OnExit()
{
    SetVisible(false);
}

void Ball::Drop()
{
    GetPhysicsBody()->SetActive(true);
    mIsDragging = false;
    mIsMarked = false;
    mMarkerGraphic.reset();
}

void Ball::SetAngularVelocity(float aSpeed)
{
    GetPhysicsBody()->SetAngularVelocity(aSpeed);
}

void Ball::Collide(Geom* aEntity, const og::physics::Contact& aContact)
{
    switch (aEntity->GetTag())
    {
    case Geom::e_walkable:
        StartWalk();
        break;
    case Geom::e_detaching:
        break;
    case Geom::e_stopsign:
        ReverseWalk();
        break;
    case Geom::e_unwalkable:
        StopWalk();
        break;
    case Geom::e_deadly:
        SetVisible(false);
        break;
    case Geom::e_unknown:
        break;
    }

    if (mDelay == 0 && IsWalking())
    {
        if (aEntity->IsLine())
        {
            auto normal = aContact.GetNormal();
            if (normal.y() < 0.5f)
            {
                if (normal.x() > 0)
                {
                    SetWalkDirection(false);
                }
                else
                {
                    SetWalkDirection(true);
                }

                mDelay = 5;
            }
        }
    }
}

void Ball::Collide(Ball* aEntity, const og::physics::Contact& /*aContact*/)
{
    if (IsWalking())
    {
        if (aEntity->IsAttached())
        {
            StopWalk();
            SetWalkSpeed(0);
            GetPhysicsBody()->SetActive(false);
            StartClimb(aEntity);
        }
        else if (mDelay == 0)
        {
            SetWalkDirection(aEntity->GetWalkDirection());
        }
    }
}

void Ball::Collide(og::Entity *aEntity, const og::physics::Contact& aContact)
{
    if (auto geom = dynamic_cast<Geom*>(aEntity))
    {
        Collide(geom, aContact);
    }
    else if (auto ball = dynamic_cast<Ball*>(aEntity))
    {
        Collide(ball, aContact);
    }
}

void Ball::LastUpdate()
{
    for (auto ce = GetPhysicsBody()->GetContactEdge(); !ce.IsEmpty(); ce = ce.GetNext())
    {
        auto c = ce.GetContact();
        if (c.GetPointCount() != 0)
        {
            Collide(static_cast<og::Entity*>(ce.GetUserData()), c);
        }
    }

    auto pos = GetPhyPosition() / GetScene()->GetPhysicsEngine()->GetRatio();
    SetPosition(pos.x(), -pos.y());
}

QVector2D Ball::NextClimbPosition()
{
    mPassedPath += GetClimbSpeed();
    auto v = mEndBall->GetPhyPosition() - mStartBall->GetPhyPosition();
    if (mPassedPath > v.length())
    {
        mStartBall = mEndBall;
        mEndBall = mPathFinder->Next();
        if (!mEndBall)
        {
            mPathFinder->ShoulCheckNearest = !mStartBall->IsNearest();
            mPathFinder->Depth = mStartBall->IsNearest() ? 5 : 0;
            mPathFinder->Init(mStartBall);
            mStartBall = mPathFinder->Start();
            mEndBall = mPathFinder->Next();
        }

        mPassedPath = 0;
        return mStartBall->GetPhyPosition();
    }

    v.normalize();
    v *= mPassedPath;
    return mStartBall->GetPhyPosition() + v;
}

void Ball::Update()
{
    if (mMarkerGraphic)
    {
        mMarkerGraphic->Update();
    }

    if (mDelay != 0)
    {
        --mDelay;
    }

    if (mCollideWith)
    {
        using namespace og::physics;
        Shape::Transform tA = {GetPhyPosition(), 0};
        Shape::Transform tB = {mCollideWith->GetPosition(), 0};
        if (!Shape::Collide(GetShape(), tA, mCollideWith->GetShape(), tB))
        {
            mCollideWith = nullptr;
        }
    }

    if (IsClimbing())
    {
        SetPhyPosition(NextClimbPosition());
    }
}

void Ball::Debug(QPainter& aPainter)
{
    if (!GetVisible())
    {
        return;
    }

    using namespace og::physics;
    auto& pos = GetPosition();
    auto a = -GetPhysicsBody()->GetAngle();
    float radius = 0;
    if (GetShape().GetType() == Shape::e_polygon)
    {
        radius = GetShape().ToPolygonShape().GetWidth();
    }
    else
    {
        radius = GetShape().GetRadius();
    }

    radius /= GetScene()->GetPhysicsEngine()->GetRatio();

    float x1 = pos.x();
    float y1 = pos.y();
    float x2 = pos.x() + radius * std::cos(a);
    float y2 = pos.y() + radius * std::sin(a);

    aPainter.save();
    aPainter.setPen(Qt::green);
    if (GetShape().GetType() == Shape::e_polygon)
    {
        auto h = GetShape().ToPolygonShape().GetHeight() / GetScene()->GetPhysicsEngine()->GetRatio();
        aPainter.save();
        aPainter.translate(x1, y1);
        aPainter.rotate(-qRadiansToDegrees(a));
        aPainter.drawRect(-radius, -h, radius + radius, h + h);
        aPainter.restore();
    }
    else
    {
        aPainter.drawEllipse(QPointF(x1, y1), radius, radius);
    }

    aPainter.drawLine(x1, y1, x2, y2);
    aPainter.restore();
}

void Ball::Render(QPainter& aPainter)
{
    if (mMarkerGraphic)
    {
        mMarkerGraphic->Render(aPainter, GetPosition());
    }

    Entity::Render(aPainter);

    if (flag & DEBUG)
    {
        Debug(aPainter);
    }
}

void Ball::StartClimb(Ball* aBall)
{
    SetClimbSpeed(GetStartClimbSpeed());
    SetClimb(true);
    mStartBall = this;
    mEndBall = aBall;
}

void Ball::RemoveStrand(Strand* aStrand)
{
    auto it = std::find(mStrand.begin(), mStrand.end(), aStrand);
    if (it != mStrand.end())
    {
        mStrand.erase(it);
    }
}

void Ball::BuildPath(Ball* aBall, bool aCheckNearest)
{
    std::unordered_set<Ball*> discovered;
    std::stack<Ball*> s;
    s.push(aBall);
    while (!s.empty())
    {
        auto v = s.top();
        s.pop();
        if (discovered.count(v) == 0)
        {
            discovered.insert(v);
            mPath.push(v);
            if (aCheckNearest && v->IsNearest())
            {
                break;
            }
            for (size_t i = 0; i < v->StrandCount(); ++i)
            {
                auto strand = v->GetStrand(i);
                auto ball = strand->GetBallA();
                if (ball == v)
                {
                    ball = strand->GetBallB();
                }

                s.push(ball);
            }
        }
    }
}

void Ball::StartWalk()
{
    if (IsWalking() || IsAttached() || IsSleeping() || mIsMarked || IsClimbing())
    {
        return;
    }

    if (GetWalkSpeed() == 0)
    {
        SetWalkSpeed(GetStartWalkSpeed());
    }

    auto speed = GetWalkDirection() ? GetWalkSpeed() :-GetWalkSpeed();
    SetAngularVelocity(speed);
}
