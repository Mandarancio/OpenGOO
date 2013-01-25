#ifndef OG_PHYSICSENGINE_H
#define OG_PHYSICSENGINE_H

#include "common.h"
#include "og_physicsbody.h"

class OGPhysicsEngine
{
public:
    static OGPhysicsEngine* GetInstance(void);
    static void DestroyInstance(void) { if (instance_) delete instance_; }

    bool Initialize();
    void SetGravity(float32 x, float32 y) { gravity_.Set(x, y); }
    void SetSimulation(int32 velocityIterations, int32 positionIterations
                       , float32 steps
            );

    void SetSleep(bool sleep) { isSleep_ = sleep; }

    void Simulate();

    void CreateBody(OGPhysicsBody*  body);

private:
    static OGPhysicsEngine* instance_;
    b2World* world_;
    b2Vec2 gravity_;
    float32 timeStep_;
    int32 velocityIterations_;
    int32 positionIterations_;
    bool isSleep_;

    OGPhysicsEngine() : world_(0), isSleep_(false) { }
    ~OGPhysicsEngine();
};

#endif // OG_PHYSICSENGINE_H
