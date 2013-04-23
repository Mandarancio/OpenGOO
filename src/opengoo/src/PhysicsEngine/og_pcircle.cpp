#include "og_pcircle.h"

#include "circle.h"

OGPCircle::OGPCircle(const Circle &circle)
    : OGPhysicsBody(circle.center())
{
}
