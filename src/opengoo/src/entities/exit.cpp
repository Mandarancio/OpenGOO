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

struct Exit::Impl
{
    int balls;
    bool isClosed;
};

Exit::Exit(const WOGLevelExit& a_exit, ExitEventListener* a_listener)
    : Entity(0, 0),
      _pImpl(new Impl),
      m_listener(a_listener)
{       
    // [1] Create sensor
    Circle c = Circle(a_exit.pos, a_exit.radius) / 10.0f;
    og::physics::SensorFilter f;
    f.category = PhysicsFactory::EXIT;
    f.mask = PhysicsFactory::BALL;

    m_sensor.reset(new og::physics::CircleSensor(c, this));
    m_sensor->SetFilter(f);
    PE->AddSensor(m_sensor.get());

    _pImpl->balls = 0;
    _pImpl->isClosed = false;

    m_ball = 0;
    m_isClosed = true;
}

Exit::~Exit()
{
    PE->RemoveSensor(m_sensor.get());
}

void Exit::Update()
{
    if (_pImpl->isClosed)
        return;

    QVector2D center = m_sensor->GetPosition();

    foreach (auto ball, m_balls)
    {
        if (ball->isExit())
            continue;

        auto userData = ball->GetUserData();

        if (userData && userData->isTouching)
        {
            QVector2D position = ball->GetCenter();
            QVector2D d = center - position;

            float force = 40.0f;

            if (d.lengthSquared() < FLT_EPSILON * FLT_EPSILON)
                continue;

            d.normalize();
            QVector2D F =  force * d;
            ball->ApplyForce(F, position);
        }

        if (ball->isSuction())
        {
            QVector2D position = ball->GetCenter();
            float lq = (center - position).lengthSquared();

            if (lq <= 1.0f)
            {
                _pImpl->balls++;
                ball->SetExit(true);
            }
        }
    }
}

int Exit::GetBalls() const
{
    return _pImpl->balls;
}

void Exit::Close()
{
//    _pImpl->isClosed = true;
//    PE->RemoveSensor(_pImpl->sensor.get());
//    auto game = OpenGOO::instance();
//    auto balls = game->GetWorld()->balls();

//    foreach (OGBall* ball, balls)
//    {
//        ball->SetSuction(false);
//    }

//    if (m_listener)
//        m_listener->Close();
}

void Exit::OnTriggerEnter(Fixture* a_fixture)
{
    auto userdata = a_fixture->GetBody()->GetUserData();

    if (!userdata)
        return;

    auto data = static_cast<OGUserData*>(userdata);

    if (data->IsBall())
    {
        auto ball = data->ToBall();

        if (ball->IsAttached() && !data->isAttachedOnEnter)
        {
            ++m_ball;
            data->isAttachedOnEnter = true;

            if (m_isClosed)
            {
                m_listener->OnOpen();
                m_isClosed = false;
            }

            data->isTouching = true;

        }
        else if (!m_isClosed && ball->IsSuckable())
        {
            data->isTouching = true;
            ball->SetSuction(true);
        }

        m_balls.push_back(ball);
    }
}

void Exit::OnTriggerExit(Fixture* a_fixture)
{
    auto userdata = a_fixture->GetBody()->GetUserData();

    if (!userdata)
        return;

    auto data = static_cast<OGUserData*>(userdata);

    if (data->IsBall())
    {
        auto ball = data->ToBall();

        if (data->isAttachedOnEnter)
        {
            --m_ball;
            data->isAttachedOnEnter = false;

            if (m_ball == 0)
            {
                m_listener->OnClosed();
                m_isClosed = true;
            }

            data->isTouching = false;
        }
        else if (ball->IsSuckable())
        {
            data->isTouching = false;
            ball->SetSuction(false);
        }

        m_balls.removeAll(ball);
    }
}
