#pragma once

#include "joint.h"

struct OGUserData;

namespace og
{
class PhysicsBody;

namespace physics
{
class DistanceJoint : public Joint
{
    og::physics::PhysicsEngine& m_physicEngine;

public:
    DistanceJoint(PhysicsEngine& a_physicEngine, og::PhysicsBody* a_b1, og::PhysicsBody* a_b2, OGUserData* a_data);

    ~DistanceJoint();
};
}
}
