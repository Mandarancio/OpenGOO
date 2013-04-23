#ifndef OG_PHYSICSBODY_H
#define OG_PHYSICSBODY_H

#include "common.h"
#include "og_physicsshape.h"

class QVector2D;

struct OGPhysicsBody
{
    enum Type {CIRCLE, POLYGON, EDGE, CHAIN};

    b2Body* body;
    b2BodyDef bodydef;
    b2Fixture* fixture;
    OGPhysicsShape* shape;

    OGPhysicsBody() : body(0), fixture(0), shape(0) {}
    OGPhysicsBody(float x, float y, bool dynamic = false, float angle = 0);
    OGPhysicsBody(const QVector2D &pos, bool dynamic = false, float angle = 0);
    virtual ~OGPhysicsBody() { delete shape; }

    QVector2D GetPosition() const;
    float GetX() const { return body->GetPosition().x; }
    float GetY() const { return body->GetPosition().y; }

    QVector2D GetVelocity() const;
    void SetVelocity(const QVector2D &v);

    void CreateFixture(float32 density = 0.0f);
    void CreateFixture(float32 density, float32 friction, float32 restitution);
    void CreateShape(Type shape);

    void ApplyForce(const QVector2D &force, const QVector2D &point);
    void ApplyForce(const b2Vec2 &force, const b2Vec2 &point);

    void SetSensor(bool sensor);
};

#endif // OG_PHYSICSBODY_H
