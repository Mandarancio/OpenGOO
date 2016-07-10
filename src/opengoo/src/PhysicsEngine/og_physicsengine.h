#pragma once

#include <memory>

#include "common.h"
#include "debug.h"


class Circle;

namespace og
{
struct PhysicsBody;
struct OGPhysicsJoint;
class OGPCircle;

namespace physics
{
class Sensor;
class ContactListener;

class PhysicsEngine
{
    static PhysicsEngine* m_instance;
    std::unique_ptr<ContactListener> m_contactListener;
    std::unique_ptr<b2World> m_world;
    b2Vec2 m_gravity;
    float32 m_timeStep;
    int32 m_velocityIterations;
    int32 m_positionIterations;
    bool m_isSleep;

    PhysicsEngine();
    ~PhysicsEngine();

    void Init();
    void Release();

public:
    static PhysicsEngine* GetInstance();
    static void DestroyInstance();

    bool Initialize(float a_x, float a_y, bool a_sleep);
    void Reload();
    void SetGravity(float a_x, float a_y);

    void SetSleep(bool a_sleep)
    {
        m_isSleep = a_sleep;
    }

    void CreateBody(og::PhysicsBody*  a_body);
    OGPCircle* CreateCircle(const Circle& a_circle);

    void CreateJoint(og::OGPhysicsJoint* a_joint);
    void DestroyJoint(og::OGPhysicsJoint* a_joint);

    void Simulate();
    void SetSimulation(int a_velIter, int a_posIter, int a_steps);

    ContactListener* GetContactListener();

    void AddSensor(Sensor* a_sensor);
    void RemoveSensor(Sensor* a_sensor);
};
}
}

typedef og::physics::PhysicsEngine PEngine;

#define PE og::physics::PhysicsEngine::GetInstance()
