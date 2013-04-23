#ifndef OG_PCIRCLE_H
#define OG_PCIRCLE_H

#include "og_physicsbody.h"

class Circle;

class OGPCircle : public OGPhysicsBody
{
public:
    OGPCircle(const Circle& circle);
};

#endif // OG_PCIRCLE_H
