#include <QVector2D>
#include <QDebug>

#include "og_physicsbody.h"
#include "og_physicsengine.h"

#include "shapefactory.h"
#include "polygonshape.h"
#include "edgeshape.h"
#include "circleshape.h"

namespace og
{
//struct PhysicsBody::ShapeFactory
//{
//    static std::unique_ptr<physics::Shape> Create(b2Shape* aShape)
//    {
//        switch (aShape->GetType())
//        {
//        case b2Shape::e_polygon:
//            return std::unique_ptr<physics::PolygonShape>(new physics::PolygonShape(static_cast<b2PolygonShape*>(aShape)));
//        case b2Shape::e_edge:
//            return std::unique_ptr<physics::EdgeShape>(new physics::EdgeShape(static_cast<b2EdgeShape*>(aShape)));
//        case b2Shape::e_circle:
//            return std::unique_ptr<physics::CircleShape>(new physics::CircleShape(static_cast<b2CircleShape*>(aShape)));
//        case b2Shape::e_chain:
//        case b2Shape::e_typeCount:
//            assert(false);
//            return std::unique_ptr<physics::Shape>(new physics::Shape(aShape));
//        }
//    }
//};

PhysicsBody::PhysicsBody(b2Body* aBody)
    : mBody(aBody)
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
    GetFixture()->SetSensor(sensor);
}

void PhysicsBody::Destroy()
{
    static_cast<physics::PhysicsEngine*>(mBody->GetUserData())->DestroyBody(mBody);
    mBody = nullptr;
}

b2Fixture* PhysicsBody::CreateFixture(const physics::FixtureDef& aDef)
{
    if (!aDef.shape)
    {
        return nullptr;
    }

    b2FixtureDef fixtureDef;
    fixtureDef.friction = aDef.friction;
    fixtureDef.restitution = aDef.restitution;
    auto shape = physics::ShapeFactory::CreateShape(aDef.shape);
    fixtureDef.shape = shape.get();
    if (!fixtureDef.shape)
    {
        return nullptr;
    }

    return mBody->CreateFixture(&fixtureDef);
}

b2Fixture* PhysicsBody::GetFixture() const
{
    return mBody->GetFixtureList();
}
}
