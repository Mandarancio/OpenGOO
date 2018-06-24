#pragma once

#include "og_physicsshape.h"

namespace og
{
namespace physics
{
class EdgeShape : public Shape
{
public:
    EdgeShape(b2EdgeShape* aShape)
        : Shape(aShape)
    {
    }

    float GetAngle() const
    {
        auto s = static_cast<const b2EdgeShape*>(GetShape());
        auto dv = s->m_vertex2 - s->m_vertex1;
        return atan2f(dv.y, dv.x);
    }
};
}
}
