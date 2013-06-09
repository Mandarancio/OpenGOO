#ifndef OG_PCIRCLE_H
#define OG_PCIRCLE_H

#include "og_physicsbody.h"
#include "circle.h"

namespace og
{
class OGPCircle : public OGPhysicsBody
{
public:
    OGPCircle(const Circle &circle) : OGPhysicsBody(circle.center()) {}
};
} // namespace og

#endif // OG_PCIRCLE_H
