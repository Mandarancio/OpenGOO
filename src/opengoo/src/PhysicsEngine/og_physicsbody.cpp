#include "og_physicsbody.h"

#include <QVector2D>

OGPhysicsBody::OGPhysicsBody(float x, float y, bool dynamic, float angle)
{
    if (dynamic) { bodydef.type = b2_dynamicBody; }

    bodydef.position.Set(x, y);
    bodydef.angle = angle;
}

OGPhysicsBody::OGPhysicsBody(const QVector2D &pos, bool dynamic, float angle)
{
    if (dynamic) { bodydef.type = b2_dynamicBody; }

    bodydef.position.Set(pos.x() , pos.y());
    bodydef.angle = angle;
}

void OGPhysicsBody::CreateFixture(float32 density)
{
    fixture = body->CreateFixture(shape->shape, density);
}

void OGPhysicsBody::CreateFixture(float32 density, float32 friction
                                  , float32 restitution
                                 )
{
    b2FixtureDef fixtureDef;

    fixtureDef.shape = shape->shape;
    fixtureDef.density = density;
    fixtureDef.friction = friction;
    fixtureDef.restitution = restitution;
    fixture = body->CreateFixture(&fixtureDef);
}

void OGPhysicsBody::CreateShape(Type type)
{
    switch (type)
    {
        case CIRCLE:
            shape = new OGPhysicsShape(b2Shape::e_circle);
            break;

        case POLYGON:
            shape = new OGPhysicsShape(b2Shape::e_polygon);
            break;

        case EDGE:
            shape = new OGPhysicsShape(b2Shape::e_edge);
            break;

        case CHAIN:
            shape = new OGPhysicsShape(b2Shape::e_chain);
            break;
    }
}

QVector2D OGPhysicsBody::GetVelocity() const
{
    float x = body->GetLinearVelocity().x;
    float y = body->GetLinearVelocity().y;

    return QVector2D(x, y);
}

void OGPhysicsBody::SetVelocity(const QVector2D& v)
{
    body->SetLinearVelocity(b2Vec2(v.x(), v.y()));
}

QVector2D OGPhysicsBody::GetPosition() const
{
    float x = body->GetPosition().x;
    float y = body->GetPosition().y;

    return QVector2D(x, y);
}

void OGPhysicsBody::ApplyForce(const QVector2D &force, const QVector2D &point)
{
    b2Vec2 f(force.x(), force.y());
    b2Vec2 p(point.x(), point.y());

    body->ApplyForce(f, p);
}

void OGPhysicsBody::ApplyForce(const b2Vec2 &force, const b2Vec2 &point)
{
    body->ApplyForce(force, point);
}

void OGPhysicsBody::SetSensor(bool sensor)
{
    fixture->SetSensor(sensor);
}
