#ifndef OG_CLIMB_H
#define OG_CLIMB_H

#include "og_behavior.h"
#include <OGPhysicsBody>

class OGClimb : public OGIClimbBehavior
{
    public:
        OGClimb(og::OGPhysicsBody* body);
        ~OGClimb();

    private:
        struct OGClimbImpl* pImpl_;

        void _Climb();
};

#endif // OG_CLIMB_H
