#ifndef OG_WALK_H
#define OG_WALK_H

#include "og_behavior.h"
#include <OGPhysicsBody>

class OGWalk : public OGIWalkBehavior
{
public:
    OGWalk(og::PhysicsBody* body);
    ~OGWalk();

private:
    struct OGWalkImpl* pImpl_;

    void _Walk();
};

#endif // OG_WALK_H
