#pragma once

#include "common.h"
#include "og_physicsshape.h"

class QVector2D;

namespace og
{
struct PhysicsBody
{
    enum Type
    {
        CIRCLE,
        POLYGON,
        EDGE,
        CHAIN
    };

    b2Body* body;
    b2BodyDef bodydef;
    b2Fixture* fixture;
    physics::Shape* shape;

    PhysicsBody() : body(0), fixture(0), shape(0) {}
    PhysicsBody(float x, float y, bool dynamic = false, float angle = 0);
    PhysicsBody(const QVector2D &pos, bool dynamic = false, float angle = 0);
    virtual ~PhysicsBody() { delete shape; }

    QVector2D GetPosition() const;
    float GetX() const { return body->GetPosition().x; }
    float GetY() const { return body->GetPosition().y; }

    QVector2D GetVelocity() const;
    void SetVelocity(const QVector2D &v);

    void CreateFixture(float32 density = 0.0f);
    void CreateFixture(float32 density, float32 friction, float32 restitution);
    void CreateShape(physics::Shape::Type shape);

    void ApplyForce(const QVector2D &force, const QVector2D &point);
    void ApplyForce(const b2Vec2 &force, const b2Vec2 &point);

    void SetSensor(bool sensor);
};
}
