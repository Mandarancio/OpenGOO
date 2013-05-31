#ifndef OG_PCIRCLE_H
#define OG_PCIRCLE_H

#include "og_physicsbody.h"

class Circle;

namespace og
{
class OGPCircle : public OGPhysicsBody
{
public:
    OGPCircle(const Circle& circle);
};
} // namespace og

#endif // OG_PCIRCLE_H
