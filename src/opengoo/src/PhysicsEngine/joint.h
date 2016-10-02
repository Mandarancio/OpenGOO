#pragma once

#include <QVector2D>

struct b2JointDef;
class b2Joint;

namespace og
{
namespace physics
{

class PhysicsEngine;

class Joint
{
public:
    virtual ~Joint()
    {
    }

    QVector2D GetAnchorA() const;
    QVector2D GetAnchorB() const;

protected:
    Joint()
        : m_jointdef(nullptr)
        , m_joint(nullptr)
    {
    }


protected:
    b2JointDef* m_jointdef;
    b2Joint* m_joint;

    friend class PhysicsEngine;
};

}
}
