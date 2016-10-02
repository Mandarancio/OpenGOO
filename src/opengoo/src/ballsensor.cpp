#include "ballsensor.h"
#include "entities/og_ball.h"
#include "physics.h"
#include "og_userdata.h"

#include <OGPhysicsEngine>

BallSensor::BallSensor(OGBall* b)
    : CircleSensor(Circle(b->GetPhyPosition(), b->GetPhyRadius() + 5), nullptr)
    , m_ball(b)
{
    SetCategory(PhysicsFactory::SENSOR);
    SetMask(PhysicsFactory::BALL);
    PE->AddSensor(this);
}

BallSensor::~BallSensor()
{
    PEngine::GetInstance()->RemoveSensor(this);
}

void BallSensor::BeginContact(Fixture* a_fixture)
{
    auto ud = a_fixture->GetBody()->GetUserData();

    if (!ud)
        return;

    auto data = static_cast<OGUserData*>(ud);
    assert(data->IsBall());
    auto ball = data->ToBall();

    if (ball->IsAttached())
    {
        m_ball->touching();
    }
}

void BallSensor::update()
{
    auto p = m_ball->GetPhyPosition();
    b2Vec2 pos(p.x(), p.y());
    m_ball->GetBody()->body->SetTransform(pos, 0);
}
