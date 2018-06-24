#pragma once

#include "og_physicsshape.h"

namespace og
{
namespace physics
{
class CircleShape : public Shape
{
public:
    CircleShape(b2CircleShape* aShape)
        : Shape(aShape)
    {
    }

    QVector2D GetPosition() const
    {
        return QVector2D(static_cast<b2CircleShape*>(GetShape())->m_p.x,
                         static_cast<b2CircleShape*>(GetShape())->m_p.y);
    }
};
}
}
