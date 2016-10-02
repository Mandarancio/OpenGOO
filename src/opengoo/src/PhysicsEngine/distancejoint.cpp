#include "distancejoint.h"

#include "og_physicsbody.h"
#include "og_physicsengine.h"

namespace og
{
namespace physics
{
DistanceJoint::DistanceJoint(og::PhysicsBody* a_b1, og::PhysicsBody* a_b2, OGUserData* a_data)
{
    b2DistanceJointDef def;
    def.Initialize(a_b1->body, a_b2->body, a_b1->body->GetPosition(), a_b2->body->GetPosition());
    m_jointdef = &def;
    PE->CreateJoint(this);
    m_joint->SetUserData(a_data);
}

DistanceJoint::~DistanceJoint()
{
    PE->DestroyJoint(this);
}
}
}
