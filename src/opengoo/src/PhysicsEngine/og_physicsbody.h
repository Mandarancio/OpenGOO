#ifndef OG_PHYSICSBODY_H
#define OG_PHYSICSBODY_H

#include "common.h"
#include "og_physicsshape.h"

struct OGPhysicsBody
{
    enum Type {CIRCLE, POLYGON, EDGE, CHAIN};

    b2Body* body;
    b2BodyDef bodydef;
    b2Fixture* fixture;
    OGPhysicsShape* shape;

    OGPhysicsBody() : body(0), fixture(0), shape(0) {}
    OGPhysicsBody(float32 x, float32 y, bool dynamic, float32 angle=0);
    ~OGPhysicsBody() { delete shape; }

    void CreateFixture(float32 density);
    void CreateFixture(float32 density, float32 friction, float32 restitution);
    void CreateShape(Type shape);    
};

#endif // OG_PHYSICSBODY_H
