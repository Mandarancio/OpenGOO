#ifndef OG_PHYSICSJOINT_H
#define OG_PHYSICSJOINT_H

#include "common.h"

struct OGPhysicsJoint
{
    b2JointDef* jointdef;
    b2Joint* joint;

    OGPhysicsJoint() : joint(0), jointdef(0) { }
    ~OGPhysicsJoint() { delete jointdef; }
};

#endif // OG_PHYSICSJOINT_H
