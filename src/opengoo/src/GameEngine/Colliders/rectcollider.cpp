#include <QtMath>

#include <GameEngine/entity.h>

#include "physics.h"

#include "rectcollider.h"

namespace og
{
RectCollider::RectCollider(float aWidth, float aHeight)
{
    m_shape.SetAsBox(aWidth * 0.5f, aHeight * 0.5f);
}

RectCollider::RectCollider(float aWidth, float aHeight, float aCenterX, float aCenterY, float aAngle)
{
    b2Vec2 c(aCenterX, aCenterY);
    m_shape.SetAsBox(aWidth * 0.5f, aHeight * 0.5f, c, qDegreesToRadians(aAngle));
}

bool RectCollider::OverlapPoint(const QVector2D& a_point) const
{
    auto& pos = GetEntity().GetPosition();
    b2Vec2 p(pos.x(), -pos.y());
    b2Rot r(qDegreesToRadians(-GetEntity().GetGraphic()->GetAngle()));
    b2Transform t(p, r);
    p.Set(a_point.x(), a_point.y());

    return TestPoint(t, p);
}
}
