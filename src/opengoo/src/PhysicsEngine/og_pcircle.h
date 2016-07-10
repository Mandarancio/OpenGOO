#pragma once

#include "og_physicsbody.h"
#include "circle.h"

namespace og
{
class OGPCircle : public PhysicsBody
{
public:
    OGPCircle(const Circle &circle)
        : PhysicsBody(circle.center())
    {
    }

    void SetRadius(float32 a_radius)
    {
        shape->SetRadius(a_radius);
    }
};
}
