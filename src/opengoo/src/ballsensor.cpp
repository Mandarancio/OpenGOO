#include "ballsensor.h"
#include "og_ball.h"
#include "physics.h"
#include "og_userdata.h"

#include <OGPhysicsEngine>

BallSensor::BallSensor(OGBall* b)
    : OGCircleSensor(Circle(b->GetCenter(), b->getRadius() + 5))
    , _ball(b)
{
    SetCategory(physics::SENSOR);
    SetMask(physics::BALL);
    PEngine::GetInstance()->AddSensor(this);
}

BallSensor::~BallSensor()
{
    PEngine::GetInstance()->RemoveSensor(this);
}

void BallSensor::_BeginContact(Fixture *fixture)
{
    auto data = fixture->GetBody()->GetUserData();

    if (data)
    {
        auto ud = OGUserData::GetUserData(data);

        if (ud->data)
        {
            auto ball = static_cast<OGBall*>(ud->data);

            if (ball->IsAttached())
            {
                _ball->touching();
            }
        }
    }
}

void BallSensor::update()
{
    pBody_->body->SetTransform(_ball->GetBodyPosition(), 0);
}
