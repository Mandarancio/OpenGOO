#ifndef OG_PHYSICSENGINE_H
#define OG_PHYSICSENGINE_H

#include "common.h"
#include "og_physicsbody.h"
#include "og_physicsjoint.h"
#include "debug.h"

class OGPhysicsEngine
{
public:
    static OGPhysicsEngine* GetInstance(void);
    static void DestroyInstance(void) { if (instance_) delete instance_; }

    bool Initialize();
    void SetGravity(float32 x, float32 y) { gravity_.Set(x, y); }
    void SetSimulation(int32 velocityIterations, int32 positionIterations
                       , int steps);

    void SetSleep(bool sleep) { isSleep_ = sleep; }

    void CreateBody(OGPhysicsBody*  body);
    void CreateJoint(OGPhysicsJoint* joint);

    void DestroyJoint(OGPhysicsJoint* joint)
    {
        world_->DestroyJoint(joint->joint);
        delete joint;
    }

    void Simulate();

private:
    static OGPhysicsEngine* instance_;
    b2World* world_;
    b2Vec2 gravity_;
    float32 timeStep_;
    int32 velocityIterations_;
    int32 positionIterations_;
    bool isSleep_;

    OGPhysicsEngine();
    ~OGPhysicsEngine();
};

#endif // OG_PHYSICSENGINE_H
