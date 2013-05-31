#include "og_physicsengine.h"
#include "og_physicsjoint.h"
#include "og_pcircle.h"
#include "og_contactlistener.h"

#include "circle.h"

using namespace og;

OGPhysicsEngine* OGPhysicsEngine::pInstance_ = 0;

OGPhysicsEngine::OGPhysicsEngine()
{
    pWorld_ = 0;
    pContactListener_ = 0;
    isSleep_ = false;
}

OGPhysicsEngine::~OGPhysicsEngine()
{
    _Release();
}

OGPhysicsEngine* OGPhysicsEngine::GetInstance()
{
    if (!pInstance_) pInstance_ = new OGPhysicsEngine;

    return pInstance_;
}

void OGPhysicsEngine::DestroyInstance(void)
{
    if (pInstance_) delete pInstance_;

    pInstance_ = 0;
}

bool OGPhysicsEngine::Initialize(float x, float y, bool sleep)
{
    if (pWorld_) return true;

    gravity_.Set(x, y);
    isSleep_ = sleep;
    _Init();

    return true;
}

void OGPhysicsEngine::Reload()
{
    _Release();
    _Init();
}

void OGPhysicsEngine::SetGravity(float x, float y)
{
    gravity_.Set(x, y);
    pWorld_->SetGravity(gravity_);
}

void OGPhysicsEngine::SetSimulation(int velIter, int posIter, int steps)
{
    timeStep_ = 1.0f / steps;
    velocityIterations_ = velIter;
    positionIterations_ = posIter;
}

void OGPhysicsEngine::Simulate()
{
    pWorld_->Step(timeStep_, velocityIterations_, positionIterations_);
}

void OGPhysicsEngine::CreateBody(OGPhysicsBody* body)
{
    body->body = pWorld_->CreateBody(&body->bodydef);
}

void OGPhysicsEngine::CreateJoint(OGPhysicsJoint* joint)
{
    joint->joint = pWorld_->CreateJoint(joint->jointdef);
}

void OGPhysicsEngine::DestroyJoint(OGPhysicsJoint* joint)
{
    pWorld_->DestroyJoint(joint->joint);
    delete joint;
}

OGPCircle* OGPhysicsEngine::CreateCircle(const Circle &circle)
{
    OGPCircle* body = new OGPCircle(circle);
    CreateBody(body);
    body->CreateShape(OGPhysicsBody::CIRCLE);
    body->shape->SetRadius(circle.radius());
    body->CreateFixture();

    return body;
}

OGContactListener* OGPhysicsEngine::GetContactListener()
{
    if (!pContactListener_) pContactListener_ = new OGContactListener;

    return pContactListener_;
}

void OGPhysicsEngine::AddSensor(OGSensor* sensor)
{
    pContactListener_->AddSensor(sensor);
}

void OGPhysicsEngine::RemoveSensor(const QString &id)
{
    pContactListener_->RemoveSensor(id);
}

void OGPhysicsEngine::_Init()
{
    pWorld_ = new b2World(gravity_);
    pWorld_->SetAllowSleeping(isSleep_);
    pContactListener_ = GetContactListener();
    pWorld_->SetContactListener(pContactListener_);
}

void OGPhysicsEngine::_Release()
{
    delete pWorld_;
    pWorld_ = 0;

    delete pContactListener_;
    pContactListener_ = 0;

}
