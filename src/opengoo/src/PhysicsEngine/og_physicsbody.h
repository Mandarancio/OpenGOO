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

struct Contact
{
    Contact(const b2Contact* aContact)
        : mContact(aContact)
    {
        InitWorldManifold();
    }

    QVector2D GetPoint(unsigned int aIndex) const
    {
        return QVector2D(mWorldManifold.points[aIndex].x, mWorldManifold.points[aIndex].y);
    }

    int GetPointCount() const
    {
        return mContact->GetManifold()->pointCount;
    }

    QVector2D GetNormal() const
    {
        const auto& normal = mWorldManifold.normal;
        return QVector2D(normal.x, normal.y);
    }

    void InitWorldManifold()
    {
       mContact->GetWorldManifold(&mWorldManifold);
    }

private:
    const b2Contact* mContact;
    b2WorldManifold mWorldManifold;
};

struct ContactEdge
{
    ContactEdge(const b2ContactEdge* aEdge)
        : mEdge(aEdge)
    {
    }

    void* GetUserData() const
    {
        return mEdge->other->GetUserData();
    }

    bool IsEmpty() const
    {
        return !mEdge;
    }

    ContactEdge GetNext() const
    {
        return ContactEdge(mEdge->next);
    }

    Contact GetContact() const
    {
        return Contact(mEdge->contact);
    }

private:
    const b2ContactEdge* mEdge;
};
}

class PhysicsBody
{
    friend class og::physics::PhysicsEngine;

    struct ShapeFactory;

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

    float GetX() const
    {
        return mBody->GetPosition().x;
    }

    float GetY() const
    {
        return mBody->GetPosition().y;
    }

    void SetPosition(const QVector2D& a_position);

    QVector2D GetVelocity() const;

    void SetVelocity(const QVector2D &v);

    void ApplyForce(const QVector2D &force, const QVector2D &point);

    void ApplyForce(const b2Vec2 &force, const b2Vec2 &point);

    void ApplyForce(const QVector2D& aForce)
    {
        ApplyForce(aForce, GetPosition());
    }

    bool IsSensor() const
    {
        return mFixture->IsSensor();
    }

    void SetSensor(bool sensor);

    const physics::Shape* GetShape() const
    {
        return mShape.get();
    }

    void SetActive(bool a_flag)
    {
        mBody->SetActive(a_flag);
    }

    bool IsActive() const
    {
        return mBody->IsActive();
    }

    void SetAwake(bool a_flag)
    {
        return mBody->SetAwake(a_flag);
    }

    bool IsAwake() const
    {
        return mBody->IsAwake();
    }

    bool IsStatic() const
    {
        return mBody->GetType() == b2_staticBody;
    }

    bool IsDynamic() const
    {
        return mBody->GetType() == b2_dynamicBody;
    }

    float GetAngle() const
    {
        return mBody->GetAngle();
    }

    void SetAngularVelocity(float aValue)
    {
        mBody->SetAngularVelocity(aValue);
    }

    physics::ContactEdge GetContactEdge() const
    {
        return physics::ContactEdge(mBody->GetContactList());
    }

    void SetUserData(void* aData)
    {
        mBody->SetUserData(aData);
    }

    void* GetUserData() const
    {
        return mBody->GetUserData();
    }

    void Destroy();

    void SetCollisionGroup(short aGroup)
    {
        auto fd = mFixture->GetFilterData();
        fd.groupIndex = aGroup;
        mFixture->SetFilterData(fd);
    }

    short GetCollisionGroup() const
    {
        return mFixture->GetFilterData().groupIndex;
    }

    void SetFriction(float aFriction)
    {
        mFixture->SetFriction(aFriction);
    }

private:
    PhysicsBody(b2Body* aBody, b2Fixture* mFixture);

private:
    b2Body* mBody;
    b2Fixture* mFixture;
    std::unique_ptr<physics::Shape> mShape;
};
}
