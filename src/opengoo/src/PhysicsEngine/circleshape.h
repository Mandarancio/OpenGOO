#pragma once

#include "og_physicsshape.h"

namespace og
{
namespace physics
{
class CircleShape : public og::physics::Shape
{
public:
    CircleShape(float aRadius)
    {
        m_type = Shape::e_circle;
        mShape.m_radius = aRadius;
    }

private:
    b2CircleShape mShape;
};
}
}
