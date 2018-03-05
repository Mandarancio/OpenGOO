#pragma once

#include "common.h"

#include "og_physicsshape.h"

class QVector2D;

namespace og
{
namespace physics
{
class PhysicsEngine;

struct FixtureDef
{
};
}

class PhysicsBody
{
    friend class og::physics::PhysicsEngine;

public:
    enum Type
    {
        CIRCLE,
        POLYGON,
        EDGE,
        CHAIN
    };

    virtual ~PhysicsBody()
    {
    }

    QVector2D GetPosition() const;
    float GetX() const { return body->GetPosition().x; }
    float GetY() const { return body->GetPosition().y; }
    void SetPosition(const QVector2D& a_position);

    QVector2D GetVelocity() const;
    void SetVelocity(const QVector2D &v);

    void CreateFixture(float32 density = 0.0f);
    void CreateFixture(float32 density, float32 friction, float32 restitution);

    void ApplyForce(const QVector2D &force, const QVector2D &point);
    void ApplyForce(const b2Vec2 &force, const b2Vec2 &point);

    void SetSensor(bool sensor);

    const physics::Shape* GetShape() const
    {
        return &mShape;
    }

    void SetActive(bool a_flag)
    {
        body->SetActive(a_flag);
    }

    bool IsActive() const
    {
        return body->IsActive();
    }

    void SetAwake(bool a_flag)
    {
        return body->SetAwake(a_flag);
    }

    bool IsAwake() const
    {
        return body->IsAwake();
    }

    bool IsStatic() const
    {
        return body->GetType() == b2_staticBody;
    }

    float GetAngle() const
    {
        return body->GetAngle();
    }

    void Destroy();

private:
    PhysicsBody(b2Body* aBody, b2Fixture* aFixture)
        : body(aBody)
        , fixture(aFixture)
        , mShape(aFixture->GetShape())
    {
    }

private:
    b2Body* body;
    b2Fixture* fixture;
    physics::Shape mShape;
};
}
