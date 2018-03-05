#include "og_physicsshape.h"

#include "common.h"

namespace og
{
namespace physics
{
bool Shape::TestPoint(const Transform& a_transform, const QVector2D& a_point) const
{
    b2Vec2 position(a_transform.position.x(), a_transform.position.y());
    b2Transform transform; transform.Set(position, a_transform.angle);
    b2Vec2 p(a_point.x(), a_point.y());
    return mShape->TestPoint(transform, p);
}
}
}
