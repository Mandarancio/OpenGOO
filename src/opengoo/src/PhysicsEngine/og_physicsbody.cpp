#include <QVector2D>
#include <QDebug>

#include "og_physicsbody.h"
#include "og_physicsengine.h"

#include "fixturebuilder.h"
#include "shapefactory.h"

namespace og
{
void PhysicsBody::CreateFixture(float32 density)
{
//    fixture = physics::FixtureBuilder(body)
//            .SetDensity(density)
//            .SetShape(shape->GetShape())
//            .Build();
}

void PhysicsBody::CreateFixture(float32 density, float32 friction, float32 restitution)
{
//    fixture = physics::FixtureBuilder(body)
//            .SetDensity(density)
//            .SetFriction(friction)
//            .SetRestitution(restitution)
//            .SetShape(shape->GetShape())
//            .Build();
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

void PhysicsBody::SetPosition(const QVector2D& a_position)
{
    b2Vec2 p(a_position.x(), a_position.y());
    auto angle = 0.0f;
    body->SetTransform(p, angle);
}

void PhysicsBody::ApplyForce(const QVector2D &force, const QVector2D &point)
{
    b2Vec2 f(force.x(), force.y());
    b2Vec2 p(point.x(), point.y());

    body->ApplyForce(f, p);
}

void PhysicsBody::ApplyForce(const b2Vec2& force, const b2Vec2& point)
{
    body->ApplyForce(force, point);
}

void PhysicsBody::SetSensor(bool sensor)
{
    fixture->SetSensor(sensor);
}

void PhysicsBody::Destroy()
{
    static_cast<physics::PhysicsEngine*>(body->GetUserData())->DestroyBody(body);
    body = nullptr;
    fixture = nullptr;
}
}
