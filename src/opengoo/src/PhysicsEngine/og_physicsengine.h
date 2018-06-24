#pragma once

#include <memory>

#include "common.h"
#include "debug.h"

#include "OGLib/point.h"

#include "og_contactlistener.h"
#include "og_physicsbody.h"
#include "og_physicsshape.h"
#include "joint.h"

class Circle;

namespace og
{
class OGPCircle;

namespace physics
{
class Sensor;
class ContactListener;
class Joint;
class CircleShape;

struct BodyDef
{
    typedef oglib::PointF Position;

    enum Type
    {
        e_unknown,
        e_static,
        e_kinematic,
        e_dynamic
    };

    Type type;
    Position position;
    float angle;
    FixtureDef fixture;

    BodyDef()
        : type(e_unknown)
        , angle(0)
    {
    }
};

struct JointDef
{
    Joint::Type type;

    JointDef()
        : type(Joint::e_unknown)
    {
    }
};

struct JointFactory
{
    JointFactory(b2World* aWorld)
        : mWorld(aWorld)
    {
    }

    b2DistanceJoint* CreateDistanceJoint(b2Body* aB1, b2Body* aB2)
    {
        b2DistanceJointDef def;
        def.Initialize(aB1, aB2, aB1->GetPosition(), aB2->GetPosition());

        return static_cast<b2DistanceJoint*>(CreateJoint(&def));
    }

    b2Joint* CreateJoint(b2Body* aB1, b2Body* aB2, const JointDef& aDef)
    {
        b2Joint* joint = nullptr;
        switch (aDef.type)
        {
        case Joint::e_distance:
            joint = CreateDistanceJoint(aB1, aB2);
            break;
        default:
            assert(false);
            break;
        }

        return joint;
    }

private:
    b2Joint* CreateJoint(const b2JointDef* aDef)
    {
        return mWorld->CreateJoint(aDef);
    }

    b2World* mWorld;
};

class PhysicsEngine
{
    std::unique_ptr<ContactListener> m_contactListener;
    std::unique_ptr<b2World> m_world;
    float32 m_timeStep;
    int32 m_velocityIterations;
    int32 m_positionIterations;
    float mRatio;

public:
    PhysicsEngine(float a_x, float a_y, bool a_sleep, float aRatio);

    void SetGravity(float a_x, float a_y);

    std::unique_ptr<PhysicsBody> CreateBody(const BodyDef& aDef);

    void DestroyBody(b2Body* aBody);

    std::unique_ptr<OGPCircle> CreateCircle(const Circle&, bool);

    std::unique_ptr<Joint> CreateJoint(PhysicsBody* aB1, PhysicsBody* aB2, const JointDef& aDef);

    void DestroyJoint(b2Joint* aJoint);

    void Simulate();
    void SetSimulation(int a_velIter, int a_posIter, int a_steps);

    void AddSensor(Sensor* a_sensor);
    void RemoveSensor(Sensor* a_sensor);

    float GetRatio() const
    {
        return mRatio;
    }

    static std::unique_ptr<PhysicsEngine> Create()
    {
        std::unique_ptr<PhysicsEngine> engine(new PhysicsEngine(0, 0, true, 1.0f/32));
        engine->SetSimulation(6, 2, 60);
        return engine;
    }

    b2Fixture* CreateFixture(const FixtureDef &aDef, PhysicsBody* aBody);

private:
    PhysicsEngine(const PhysicsEngine&);
    PhysicsEngine& operator=(const PhysicsEngine&);

    bool InitBodyType(BodyDef::Type aType, b2BodyDef* aOut);

private:
    JointFactory mJointFactory;
};
}
}

typedef og::physics::PhysicsEngine PEngine;
