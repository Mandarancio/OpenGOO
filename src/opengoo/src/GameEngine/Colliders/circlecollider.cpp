#include "PhysicsEngine/og_physicsbody.h"

#include "GameEngine/entity.h"

#include "physics.h"

#include "circlecollider.h"

namespace og
{
CircleCollider::CircleCollider(float a_radius)
{
    m_shape.m_radius = a_radius;
}

bool CircleCollider::OverlapPoint(const QVector2D& a_point) const
{
    auto& pos = GetEntity().GetPosition();
    b2Vec2 p(pos.x(), pos.y());
    b2Transform t;
    t.Set(p, 0.0f);
    p.Set(a_point.x(), a_point.y());

    return m_shape.TestPoint(t, p);
}

}
