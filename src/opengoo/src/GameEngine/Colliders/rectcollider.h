#pragma once

#include "GameEngine/Colliders/collider.h"

#include <Box2D/Collision/Shapes/b2PolygonShape.h>

namespace og
{
class Entity;

class RectCollider : public og::Collider
{
    bool OverlapPoint(const QVector2D& a_point) const;

public:
    RectCollider(float a_width, float a_height);

private:
    b2PolygonShape m_shape;
};
}
