#include "og_physicsengine.h"

OGPhysicsEngine* OGPhysicsEngine::instance_ = 0;

OGPhysicsEngine::OGPhysicsEngine()
{
    world_ = 0;
    isSleep_ = false;
}
OGPhysicsEngine::~OGPhysicsEngine()
{
    delete world_;
}

bool OGPhysicsEngine::Initialize(float32 x, float32 y, bool sleep)
{
    if (world_) { return false; }

    gravity_.Set(x, y);
    isSleep_ = sleep;

    world_ = new b2World(gravity_);
    world_->SetAllowSleeping(isSleep_);

    return true;
}

void OGPhysicsEngine::Reload()
{
    delete world_;

    world_ = new b2World(gravity_);
    world_->SetAllowSleeping(isSleep_);
}

OGPhysicsEngine* OGPhysicsEngine::GetInstance()
{
    if (!instance_) { instance_ = new OGPhysicsEngine; }

    return instance_;
}

void OGPhysicsEngine::SetGravity(float32 x, float32 y)
{
    gravity_.Set(x, y);
    world_->SetGravity(gravity_);
}

void OGPhysicsEngine::SetSimulation(int32 velocityIterations
                                      , int32 positionIterations
                                      , int steps)
{
    timeStep_ = 1.0f/steps;
    velocityIterations_ = velocityIterations;
    positionIterations_ = positionIterations;
}

void OGPhysicsEngine::Simulate()
{
    world_->Step(timeStep_, velocityIterations_, positionIterations_);
}

void OGPhysicsEngine::CreateBody(OGPhysicsBody* body)
{   
    body->body = world_->CreateBody(&body->bodydef);
}

void OGPhysicsEngine::CreateJoint(OGPhysicsJoint* joint)
{
    joint->joint = world_->CreateJoint(joint->jointdef);
}
