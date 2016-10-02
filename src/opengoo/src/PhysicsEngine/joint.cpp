#include <Box2D/Dynamics/Joints/b2Joint.h>

#include "joint.h"

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

}
}
