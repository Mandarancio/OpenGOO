#include "distancejoint.h"

#include "og_physicsbody.h"
#include "og_physicsengine.h"

namespace og
{
namespace physics
{
DistanceJoint::DistanceJoint(og::physics::PhysicsEngine& a_physicEngine,
                             og::PhysicsBody* a_b1,
                             og::PhysicsBody* a_b2,
                             OGUserData* a_data)
    : m_physicEngine(a_physicEngine)
{
//    b2DistanceJointDef def;
//    def.Initialize(a_b1->body, a_b2->body, a_b1->body->GetPosition(), a_b2->body->GetPosition());
//    m_jointdef = &def;
//    m_physicEngine.CreateJoint(this);
//    m_joint->SetUserData(a_data);
}

DistanceJoint::~DistanceJoint()
{
    m_physicEngine.DestroyJoint(this);
}
}
}
