#ifndef OG_PHYSICSENGINE_H
#define OG_PHYSICSENGINE_H

#include "common.h"
#include "debug.h"

class Circle;

namespace og
{

class OGPCircle;
struct OGPhysicsBody;
struct OGPhysicsJoint;

class OGContactListener;
class OGSensor;

class OGPhysicsEngine
{
    public:
        static OGPhysicsEngine* GetInstance(void);
        static void DestroyInstance(void);

        bool Initialize(float x, float y, bool sleep);
        void Reload();
        void SetGravity(float x, float y);

        void SetSleep(bool sleep) { isSleep_ = sleep; }

        void CreateBody(OGPhysicsBody*  body);
        OGPCircle* CreateCircle(const Circle &circle);

        void CreateJoint(OGPhysicsJoint* joint);
        void DestroyJoint(OGPhysicsJoint* joint);

        void Simulate();
        void SetSimulation(int velIter, int posIter, int steps);

        OGContactListener* GetContactListener();

        void AddSensor(OGSensor* sensor);
        void RemoveSensor(const QString &id);

    private:
        static OGPhysicsEngine* pInstance_;
        b2World* pWorld_;
        b2Vec2 gravity_;
        float32 timeStep_;
        int32 velocityIterations_;
        int32 positionIterations_;
        bool isSleep_;

        OGContactListener* pContactListener_;

        OGPhysicsEngine();
        ~OGPhysicsEngine();

        void _Init();
        void _Release();
};
} // namespace og

typedef og::OGPhysicsEngine PEngine;

#endif // OG_PHYSICSENGINE_H
