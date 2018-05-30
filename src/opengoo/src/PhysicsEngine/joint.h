#pragma once

#include <QVector2D>
#include "common.h"

namespace og
{
class Entity;

namespace physics
{
class PhysicsEngine;

class Joint
{
public:
    enum Type
    {
        e_unknown,
        e_distance
    };

public:
    Joint(b2Joint* aJoint)
        : m_joint(aJoint)
    {
        switch (m_joint->GetType())
        {
        case e_distanceJoint:
            mType = e_distance;
            break;
        default:
            assert(false);
            mType = e_unknown;
            break;
        }
    }

    virtual ~Joint()
    {
    }

    QVector2D GetAnchorA() const;
    QVector2D GetAnchorB() const;

    Type GetType() const
    {
        return mType;
    }

    void Destroy();

    bool IsNull() const
    {
        return m_joint;
    }

    og::Entity* GetBodyA() const;

    og::Entity* GetBodyB() const;

private:
    Joint(const Joint&);
    Joint& operator=(const Joint&);

protected:
    Type mType;
    b2Joint* m_joint;
};
}
}
