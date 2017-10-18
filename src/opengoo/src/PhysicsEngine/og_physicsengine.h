#pragma once

#include <memory>

#include "common.h"
#include "debug.h"

#include "og_contactlistener.h"

class Circle;

namespace og
{
class PhysicsBody;
class OGPCircle;

namespace physics
{
class Sensor;
class ContactListener;
class Joint;

class PhysicsEngine
{
    std::unique_ptr<ContactListener> m_contactListener;
    std::unique_ptr<b2World> m_world;
    float32 m_timeStep;
    int32 m_velocityIterations;
    int32 m_positionIterations;

public:

    PhysicsEngine(float a_x, float a_y, bool a_sleep);

    void SetGravity(float a_x, float a_y);

    void CreateBody(og::PhysicsBody* a_body);
    OGPCircle* CreateCircle(const Circle& a_circle);

    void CreateJoint(og::physics::Joint* a_joint);
    void DestroyJoint(og::physics::Joint* a_joint);

    void Simulate();
    void SetSimulation(int a_velIter, int a_posIter, int a_steps);    

    void AddSensor(Sensor* a_sensor);
    void RemoveSensor(Sensor* a_sensor);

private:

    PhysicsEngine(const PhysicsEngine&);
    PhysicsEngine& operator=(const PhysicsEngine&);
};
}
}

typedef og::physics::PhysicsEngine PEngine;
