#pragma once

#include "GameEngine/Colliders/collider.h"

#include <Box2D/Collision/Shapes/b2PolygonShape.h>

namespace og
{
class RectCollider : public og::Collider
{
protected:
    bool OverlapPoint(const QVector2D& a_point) const;

public:
    RectCollider(float aWidth, float aHeight);

    /// @param aAngle in radians
    RectCollider(float aWidth, float aHeight, float aCenterX, float aCenterY, float aAngle=0);

    bool TestPoint(const b2Transform& xf, const b2Vec2& p) const
    {
        return m_shape.TestPoint(xf, p);
    }

private:
    b2PolygonShape m_shape;
};
}
