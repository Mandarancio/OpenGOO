#include "exit.h"

#include "exiteventlistener.h"
#include "ball.h"

Exit::Exit(std::unique_ptr<og::PhysicsBody> aPhysicsBody)
    : Entity(0, 0)
    , mIsClosed(true)
{
    SetPhysicsBody(std::move(aPhysicsBody));
}

void Exit::CollideWith(Ball* aBall)
{
    const float force = 40.0f;
    auto v = GetPhysicsBody()->GetPosition() - aBall->GetPosition();
    if (v.length() < std::numeric_limits<float>::epsilon())
    {
        return;
    }

    v.normalize();
    v *= force;
    aBall->GetPhysicsBody() ->ApplyForce(v);
}

void Exit::LastUpdate()
{
    bool isOpen = false;
    for (auto ce = GetPhysicsBody()->GetContactEdge(); !ce.IsEmpty(); ce = ce.GetNext())
    {
        auto e = static_cast<og::Entity*>(ce.GetUserData());
        if (auto ball = dynamic_cast<Ball*>(e))
        {
            if (!isOpen && ball->IsAttached())
            {
                isOpen = true;
                ce = GetPhysicsBody()->GetContactEdge();
            }

            if (isOpen)
            {
                CollideWith(ball);
            }
        }
    }

    if (IsClosed() && isOpen)
    {
        mIsClosed = false;
        for (auto it = mListener.begin(); it != mListener.end(); ++it)
        {
            (*it)->OnOpen();
        }
    }
    else if (IsOpen() && !isOpen)
    {
        mIsClosed = true;
        for (auto it = mListener.begin(); it != mListener.end(); ++it)
        {
            (*it)->OnClosed();
        }
    }
}

void Exit::Update()
{
//    if (mIsClosed)
//        return;

//    auto center = m_sensor->GetPosition();

//    for (int i = 0; i < m_balls.size(); ++i)
//    {
//        auto userData = GetBall(i)->GetUserData();

//        if (userData && userData->isTouching)
//        {
//            auto position = GetBall(i)->GetPhyPosition();
//            auto v = center - position;

//            float force = 40.0f;

//            if (v.lengthSquared() < FLT_EPSILON * FLT_EPSILON)
//                continue;

//            v.normalize();
//            v *= force;
//            GetBall(i)->GetBody()->ApplyForce(v, position);
//        }

//        if (GetBall(i)->isSuction())
//        {
//            auto position = GetBall(i)->GetPhyPosition();
//            float lq = (center - position).lengthSquared();

//            if (lq <= 1.0f)
//            {
//                GetBall(i)->SetExit(true);
//                mListener->OnBallExit();
//                m_remove.push_back(GetBall(i));
//            }
//        }
//    }
}
