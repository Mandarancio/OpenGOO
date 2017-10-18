#include "og_physicsengine.h"
#include "joint.h"
#include "og_pcircle.h"

#include "circle.h"

namespace og
{
namespace physics
{

PhysicsEngine::PhysicsEngine(float a_x, float a_y, bool a_sleep)
    : m_contactListener(new ContactListener)
    , m_world(new b2World(b2Vec2(a_x, a_y)))
{
    m_world->SetAllowSleeping(a_sleep);
    m_world->SetContactListener(m_contactListener.get());
}

void PhysicsEngine::SetGravity(float a_x, float a_y)
{
    m_world->SetGravity(b2Vec2(a_x, a_y));
}

void PhysicsEngine::SetSimulation(int a_velIter, int a_posIter, int a_steps)
{
    m_timeStep = 1.0f / a_steps;
    m_velocityIterations = a_velIter;
    m_positionIterations = a_posIter;
}

void PhysicsEngine::Simulate()
{
    m_world->Step(m_timeStep, m_velocityIterations, m_positionIterations);
}

void PhysicsEngine::CreateBody(PhysicsBody* a_body)
{
    a_body->body = m_world->CreateBody(&a_body->bodydef);
}

void PhysicsEngine::CreateJoint(Joint* a_joint)
{
    a_joint->m_joint = m_world->CreateJoint(a_joint->m_jointdef);
}

void PhysicsEngine::DestroyJoint(Joint* a_joint)
{
    m_world->DestroyJoint(a_joint->m_joint);
    a_joint->m_joint = nullptr;
}

OGPCircle* PhysicsEngine::CreateCircle(const Circle& a_circle)
{
    auto body = new OGPCircle(a_circle);
    CreateBody(body);
    body->CreateShape(Shape::e_circle);
    body->SetRadius(a_circle.radius());
    body->CreateFixture();

    return body;
}

void PhysicsEngine::AddSensor(Sensor* a_sensor)
{
    m_contactListener->AddSensor(a_sensor);
}

void PhysicsEngine::RemoveSensor(Sensor* a_sensor)
{
    m_contactListener->RemoveSensor(a_sensor);
}

}
}
