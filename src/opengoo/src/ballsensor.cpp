#include "ballsensor.h"
#include "entities/og_ball.h"
#include "physics.h"
#include "og_userdata.h"

BallSensor::BallSensor(og::physics::PhysicsEngine& a_physicEngine, OGBall* b)
    : CircleSensor(a_physicEngine, Circle(b->GetPhyPosition(), b->GetPhyRadius() + 5), nullptr)
    , m_ball(b)
    , m_physicEngine(a_physicEngine)
{
    SetCategory(PhysicsFactory::SENSOR);
    SetMask(PhysicsFactory::BALL);
    m_physicEngine.AddSensor(this);
}

BallSensor::~BallSensor()
{
    m_physicEngine.RemoveSensor(this);
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
