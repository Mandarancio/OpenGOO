#pragma once

#include "og_physicsshape.h"

namespace og
{
namespace physics
{
class PolygonShape : public Shape
{
public:
    PolygonShape(b2PolygonShape* aShape)
        : Shape(aShape)
    {
    }

    float GetWidth() const
    {
        return static_cast<b2PolygonShape*>(GetShape())->m_vertices[2].x;
    }

    float GetHeight() const
    {
        return static_cast<b2PolygonShape*>(GetShape())->m_vertices[2].y;
    }
};
}
}
