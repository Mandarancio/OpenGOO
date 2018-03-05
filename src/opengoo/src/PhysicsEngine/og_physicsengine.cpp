#include "og_physicsengine.h"
#include "joint.h"
#include "og_pcircle.h"

#include "circle.h"
#include "shapefactory.h"
#include "fixturebuilder.h"

namespace og
{
namespace physics
{

PhysicsEngine::PhysicsEngine(float a_x, float a_y, bool a_sleep, float aRatio)
    : m_contactListener(new ContactListener)
    , m_world(new b2World(b2Vec2(a_x, a_y)))
    , mRatio(aRatio)
    , mJointFactory(m_world.get())
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

bool PhysicsEngine::InitBodyType(BodyDef::Type aType, b2BodyDef* aOut)
{
    if (aType == BodyDef::e_dynamic)
    {
        aOut->type = b2_dynamicBody;
    }
    else if (aType == BodyDef::e_kinematic)
    {
        aOut->type = b2_kinematicBody;
    }
    else if (aType == BodyDef::e_static)
    {
        aOut->type = b2_staticBody;
    }
    else
    {
        return false;
    }

    return true;
}

std::unique_ptr<PhysicsBody> PhysicsEngine::CreateBody(const BodyDef& aDef)
{
    b2BodyDef bodyDef;
    if (!InitBodyType(aDef.type, &bodyDef))
    {
        return nullptr;
    }

    b2FixtureDef fixtureDef;
    fixtureDef.shape = ShapeFactory::CreateShape(aDef.shape);
    if (!fixtureDef.shape)
    {
        return nullptr;
    }

    bodyDef.position.Set(aDef.position.x(), aDef.position.y());
    bodyDef.angle = aDef.angle;
    auto body = m_world->CreateBody(&bodyDef);
    body->SetUserData(this);

    FixtureBuilder builder(body);
    auto fixture = builder.SetShape(fixtureDef.shape).Build();

    return std::unique_ptr<PhysicsBody>(new PhysicsBody(body, fixture));
}

void PhysicsEngine::DestroyBody(b2Body* aBody)
{
    m_world->DestroyBody(aBody);
}

std::unique_ptr<Joint> PhysicsEngine::CreateJoint(PhysicsBody* aB1, PhysicsBody* aB2, const JointDef& aDef)
{
    if (auto j = mJointFactory.CreateJoint(aB1->body, aB2->body, aDef))
    {
        j->SetUserData(this);
        return std::unique_ptr<Joint>(new Joint(j));
    }

    return nullptr;
}

void PhysicsEngine::DestroyJoint(b2Joint *aJoint)
{
    m_world->DestroyJoint(aJoint);
}

std::unique_ptr<OGPCircle> PhysicsEngine::CreateCircle(const Circle& a_circle, bool aDynamic)
{
//    std::unique_ptr<OGPCircle> body(new OGPCircle);

//    {
//        auto pos = a_circle.center() * GetRatio();
//        PhysicsBodyBuilder builder(this);

//        if (aDynamic)
//        {
//            builder.SetType(PhysicsBodyBuilder::e_dynamic);
//        }
//        else
//        {
//            builder.SetType(PhysicsBodyBuilder::e_static);
//        }

//        body->body = SetPosition(pos.x(), pos.y()).Build();
//    }

//    body->CreateShape(Shape::e_circle);
//    body->SetRadius(a_circle.radius());
//    body->CreateFixture();

    return nullptr;
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
