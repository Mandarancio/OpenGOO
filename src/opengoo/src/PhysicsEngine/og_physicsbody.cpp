#include <QVector2D>

#include "og_physicsbody.h"


namespace og
{
PhysicsBody::PhysicsBody(float x, float y, bool dynamic, float angle)
{
    if (dynamic)
        bodydef.type = b2_dynamicBody;

    bodydef.position.Set(x, y);
    bodydef.angle = angle;
}

PhysicsBody::PhysicsBody(const QVector2D &pos, bool dynamic, float angle)
{
    if (dynamic)
        bodydef.type = b2_dynamicBody;

    bodydef.position.Set(pos.x() , pos.y());
    bodydef.angle = angle;
}

void PhysicsBody::CreateFixture(float32 density)
{
    fixture = body->CreateFixture(shape->GetShape(), density);
}

void PhysicsBody::CreateFixture(float32 density, float32 friction
                                  , float32 restitution
                                 )
{
    b2FixtureDef fixtureDef;

    fixtureDef.shape = shape->GetShape();
    fixtureDef.density = density;
    fixtureDef.friction = friction;
    fixtureDef.restitution = restitution;
    fixture = body->CreateFixture(&fixtureDef);
}

void PhysicsBody::CreateShape(physics::Shape::Type type)
{
    shape = new physics::Shape(type);
}

QVector2D PhysicsBody::GetVelocity() const
{
    float x = body->GetLinearVelocity().x;
    float y = body->GetLinearVelocity().y;

    return QVector2D(x, y);
}

void PhysicsBody::SetVelocity(const QVector2D& v)
{
    body->SetLinearVelocity(b2Vec2(v.x(), v.y()));
}

QVector2D PhysicsBody::GetPosition() const
{
    float x = body->GetPosition().x;
    float y = body->GetPosition().y;

    return QVector2D(x, y);
}

void PhysicsBody::ApplyForce(const QVector2D &force, const QVector2D &point)
{
    b2Vec2 f(force.x(), force.y());
    b2Vec2 p(point.x(), point.y());

    body->ApplyForce(f, p);
}

void PhysicsBody::ApplyForce(const b2Vec2 &force, const b2Vec2 &point)
{
    body->ApplyForce(force, point);
}

void PhysicsBody::SetSensor(bool sensor)
{
    fixture->SetSensor(sensor);
}
}
