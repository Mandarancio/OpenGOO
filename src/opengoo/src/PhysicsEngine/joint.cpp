#include "joint.h"

#include "og_physicsengine.h"

namespace og
{
namespace physics
{
QVector2D Joint::GetAnchorA() const
{
    auto a = m_joint->GetAnchorA();

    return QVector2D(a.x, a.y);
}

QVector2D Joint::GetAnchorB() const
{
    auto a = m_joint->GetAnchorB();

    return QVector2D(a.x, a.y);
}

void Joint::Destroy()
{
    static_cast<PhysicsEngine*>(m_joint->GetUserData())->DestroyJoint(m_joint);
    m_joint = nullptr;
    mType = e_unknown;
}
}
}
