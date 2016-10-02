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
public:
    DistanceJoint(og::PhysicsBody* a_b1, og::PhysicsBody* a_b2, OGUserData* a_data);

    ~DistanceJoint();
};
}
}
