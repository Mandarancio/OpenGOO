#pragma once

#include "common.h"


namespace og
{
struct OGPhysicsJoint
{
    b2JointDef* jointdef;
    b2Joint* joint;

    OGPhysicsJoint() : jointdef(0), joint(0) { }
    ~OGPhysicsJoint() { delete jointdef; }
};
}
