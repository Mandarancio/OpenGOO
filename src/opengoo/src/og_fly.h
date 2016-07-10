#ifndef OG_FLY_H
#define OG_FLY_H

#include "og_behavior.h"
#include <OGPhysicsBody>

class OGFly : public OGIFlyBehavior
{
    public:
        OGFly(og::PhysicsBody* body);
        ~OGFly();

    private:
        struct OGFlyImpl* pImpl_;

        void _Fly() {}
};

#endif // OG_FLY_H
