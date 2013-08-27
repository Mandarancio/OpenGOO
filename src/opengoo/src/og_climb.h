#ifndef OG_CLIMB_H
#define OG_CLIMB_H

#include "og_behavior.h"
#include <OGPhysicsBody>

class OGClimb : public OGIClimbBehavior
{
    public:
        OGClimb(og::OGPhysicsBody* body);
        ~OGClimb();
        void initNewTarget();
    private:
        struct OGClimbImpl* pImpl_;
        void onTargetChanged();
        void onOriginChanged();
        void _Climb();
        bool isNewTarget;
        bool isNewOrigin;
        QVector2D originCorrection;
};

#endif // OG_CLIMB_H
