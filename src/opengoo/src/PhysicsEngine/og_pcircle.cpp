#include "og_pcircle.h"

#include "circle.h"

using namespace og;

OGPCircle::OGPCircle(const Circle &circle)
    : OGPhysicsBody(circle.center())
{
}
