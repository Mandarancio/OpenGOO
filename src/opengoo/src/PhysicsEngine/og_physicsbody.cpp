#include <QVector2D>
#include <QDebug>

#include "og_physicsbody.h"
#include "og_physicsengine.h"

#include "polygonshape.h"
#include "edgeshape.h"

namespace og
{
struct PhysicsBody::ShapeFactory
{
    static std::unique_ptr<physics::Shape> Create(b2Shape* aShape)
    {
        switch (aShape->GetType())
        {
        case b2Shape::e_polygon:
            return std::unique_ptr<physics::PolygonShape>(new physics::PolygonShape(aShape));
        case b2Shape::e_edge:
            return std::unique_ptr<physics::EdgeShape>(new physics::EdgeShape(aShape));
        }

        return std::unique_ptr<physics::Shape>(new physics::Shape(aShape));
    }
};

PhysicsBody::PhysicsBody(b2Body* aBody, b2Fixture* aFixture)
    : mBody(aBody)
    , mFixture(aFixture)
    , mShape(ShapeFactory::Create(aFixture->GetShape()))
{
}

QVector2D PhysicsBody::GetVelocity() const
{
    float x = mBody->GetLinearVelocity().x;
    float y = mBody->GetLinearVelocity().y;

    return QVector2D(x, y);
}

void PhysicsBody::SetVelocity(const QVector2D& v)
{
    mBody->SetLinearVelocity(b2Vec2(v.x(), v.y()));
}

QVector2D PhysicsBody::GetPosition() const
{
    float x = mBody->GetPosition().x;
    float y = mBody->GetPosition().y;

    return QVector2D(x, y);
}

void PhysicsBody::SetPosition(const QVector2D& a_position)
{
    b2Vec2 p(a_position.x(), a_position.y());
    auto angle = 0.0f;
    mBody->SetTransform(p, angle);
}

void PhysicsBody::ApplyForce(const QVector2D &force, const QVector2D &point)
{
    b2Vec2 f(force.x(), force.y());
    b2Vec2 p(point.x(), point.y());

    mBody->ApplyForce(f, p);
}

void PhysicsBody::ApplyForce(const b2Vec2& force, const b2Vec2& point)
{
    mBody->ApplyForce(force, point);
}

void PhysicsBody::SetSensor(bool sensor)
{
    mFixture->SetSensor(sensor);
}

void PhysicsBody::Destroy()
{
    static_cast<physics::PhysicsEngine*>(mBody->GetUserData())->DestroyBody(mBody);
    mBody = nullptr;
    mFixture = nullptr;
}
}
