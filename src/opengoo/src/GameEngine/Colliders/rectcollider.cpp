#include <QtMath>

#include <GameEngine/entity.h>

#include "physics.h"

#include "rectcollider.h"

#include <QDebug>

namespace og
{
RectCollider::RectCollider(float a_width, float a_height)
{
    m_shape.SetAsBox(a_width / 2, a_height / 2);
}

bool RectCollider::OverlapPoint(const QVector2D& a_point) const
{
    auto& pos = GetEntity().GetPosition();
    b2Vec2 p(pos.x(), -pos.y());
    b2Rot r(qDegreesToRadians(-GetEntity().GetGraphic()->GetAngle()));
    b2Transform t(p, r);
    p.Set(a_point.x(), a_point.y());

    return m_shape.TestPoint(t, p);
}
}
