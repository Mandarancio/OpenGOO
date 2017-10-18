#include "exit.h"
#include "wog_exit.h"

#include <OGPhysicsEngine>
#include "circle.h"
#include "physics.h"

#include "opengoo.h"
#include "og_world.h"
#include "og_userdata.h"

#include "exiteventlistener.h"
#include "PhysicsEngine/og_circlesensor.h"


using namespace og;

Exit::Exit(og::physics::PhysicsEngine& a_physicEngine, const WOGLevelExit& a_exit, ExitEventListener* a_listener)
    : Entity(0, 0)
    , m_listener(a_listener)
    , m_physicEngine(a_physicEngine)
{       
    Circle c = Circle(a_exit.pos, a_exit.radius) / 10.0f;
    og::physics::SensorFilter f;
    f.category = PhysicsFactory::EXIT;
    f.mask = PhysicsFactory::BALL;

    m_sensor.reset(new og::physics::CircleSensor(m_physicEngine, c, this));
    m_sensor->SetFilter(f);
    m_physicEngine.AddSensor(m_sensor.get());

    m_isClosed = true;
}

Exit::~Exit()
{
    m_physicEngine.RemoveSensor(m_sensor.get());
}

void Exit::Update()
{
    if (m_isClosed)
        return;

    auto center = m_sensor->GetPosition();

    for (int i = 0; i < m_balls.size(); ++i)
    {
        auto userData = GetBall(i)->GetUserData();

        if (userData && userData->isTouching)
        {
            auto position = GetBall(i)->GetPhyPosition();
            auto v = center - position;

            float force = 40.0f;

            if (v.lengthSquared() < FLT_EPSILON * FLT_EPSILON)
                continue;

            v.normalize();
            v *= force;
            GetBall(i)->GetBody()->ApplyForce(v, position);
        }

        if (GetBall(i)->isSuction())
        {
            auto position = GetBall(i)->GetPhyPosition();
            float lq = (center - position).lengthSquared();

            if (lq <= 1.0f)
            {
                GetBall(i)->SetExit(true);
                m_listener->OnBallExit();
                m_remove.push_back(GetBall(i));
            }
        }
    }

    if (!m_remove.isEmpty())
    {
        for (int i = 0; i < m_remove.size(); ++i)
        {
            m_balls.removeOne(m_remove[i]);
        }

        m_remove.clear();
    }
}

void Exit::AddBall(OGUserData& a_data, OGBall* a_ball)
{
    assert(a_ball);

    if (a_ball->IsAttached() && !a_data.isAttachedOnEnter)
    {
        a_data.isAttachedOnEnter = true;

        if (m_isClosed)
        {
            m_listener->OnOpen();
            m_isClosed = false;
        }

        a_data.isTouching = true;

    }
    else if (!m_isClosed && a_ball->IsSuckable())
    {
        a_data.isTouching = true;
        a_ball->SetSuction(true);
    }

    assert(m_balls.indexOf(a_ball) == -1);
    m_balls.push_back(a_ball);
}

void Exit::RemoveBall(OGUserData& a_data, OGBall* a_ball)
{
    assert(a_ball);

    if (a_data.isAttachedOnEnter)
    {
        a_data.isAttachedOnEnter = false;

        if ((m_balls.size() - 1) == 0)
        {
            m_listener->OnClosed();
            m_isClosed = true;
        }

        a_data.isTouching = false;
    }
    else if (a_ball->IsSuckable())
    {
        a_data.isTouching = false;
        a_ball->SetSuction(false);
    }

    m_remove.push_back(a_ball);
}

void Exit::ProcessFixture(Fixture* a_fixture, ExitMemFn a_callback)
{
    auto userdata = a_fixture->GetBody()->GetUserData();

    if (!userdata)
        return;

    auto data = static_cast<OGUserData*>(userdata);

    if (data->IsBall())
        CALL_MEMBER_FN(*this, a_callback)(*data, data->ToBall());
}
