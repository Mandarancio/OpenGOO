#ifndef OG_WALK_H
#define OG_WALK_H

#include "og_behavior.h"

struct OGPhysicsBody;

class OGWalk : public OGIWalkBehavior
{
public:
    OGWalk(OGPhysicsBody* body);
    ~OGWalk();

private:
    struct OGWalkImpl* pImpl_;

    void _Walk();
};

#endif // OG_WALK_H
