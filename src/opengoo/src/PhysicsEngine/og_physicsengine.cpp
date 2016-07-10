#include "og_physicsengine.h"
#include "og_physicsjoint.h"
#include "og_pcircle.h"
#include "og_contactlistener.h"

#include "circle.h"

namespace og
{
namespace physics
{
PhysicsEngine* PhysicsEngine::m_instance = nullptr;

PhysicsEngine::PhysicsEngine()
{
    m_isSleep = false;
}

PhysicsEngine::~PhysicsEngine()
{
    Release();
}

PhysicsEngine* PhysicsEngine::GetInstance()
{
    if (!m_instance)
        m_instance = new PhysicsEngine;

    return m_instance;
}

void PhysicsEngine::DestroyInstance()
{
    if (m_instance)
        delete m_instance;

    m_instance = nullptr;
}

bool PhysicsEngine::Initialize(float a_x, float a_y, bool a_sleep)
{
    if (m_world)
        return true;

    m_gravity.Set(a_x, a_y);
    m_isSleep = a_sleep;
    Init();

    return true;
}

void PhysicsEngine::Reload()
{
    Release();
    Init();
}

void PhysicsEngine::SetGravity(float a_x, float a_y)
{
    m_gravity.Set(a_x, a_y);
    m_world->SetGravity(m_gravity);
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

void PhysicsEngine::CreateJoint(OGPhysicsJoint* a_joint)
{
    a_joint->joint = m_world->CreateJoint(a_joint->jointdef);
}

void PhysicsEngine::DestroyJoint(OGPhysicsJoint* a_joint)
{
    if (!a_joint)
        return;

    m_world->DestroyJoint(a_joint->joint);
    delete a_joint;
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

ContactListener* PhysicsEngine::GetContactListener()
{
    if (!m_contactListener)
        m_contactListener.reset(new ContactListener);

    return m_contactListener.get();
}

void PhysicsEngine::AddSensor(Sensor* a_sensor)
{
    m_contactListener->AddSensor(a_sensor);
}

void PhysicsEngine::RemoveSensor(Sensor* a_sensor)
{
    m_contactListener->RemoveSensor(a_sensor);
}

void PhysicsEngine::Init()
{
    m_world.reset(new b2World(m_gravity));
    m_world->SetAllowSleeping(m_isSleep);
    m_world->SetContactListener(GetContactListener());
}

void PhysicsEngine::Release()
{
    m_world.reset();
    m_contactListener.reset();
}
}
}
