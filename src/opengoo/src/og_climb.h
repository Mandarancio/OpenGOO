#ifndef OG_CLIMB_H
#define OG_CLIMB_H

#include "og_behavior.h"

struct OGPhysicsBody;

class OGClimb : public OGIClimbBehavior
{
    public:
        OGClimb(OGPhysicsBody* body);
        ~OGClimb();

    private:
        struct OGClimbImpl* pImpl_;

        void _Climb();
};

#endif // OG_CLIMB_H
