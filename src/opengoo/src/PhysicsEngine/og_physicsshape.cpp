#include "og_physicsshape.h"

#include "common.h"

#include "polygonshape.h"
#include "circleshape.h"

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

bool Shape::Collide(const Shape& aShapeA, const Transform& aTransformA, const Shape& aShapeB, const Transform& aTransformB)
{
    b2Manifold mf;
    b2Vec2 pos;
    b2Rot rot;
    if (aShapeA.GetType() == e_circle || aShapeB.GetType() == e_circle)
    {
        pos.Set(aTransformA.position.x(), aTransformA.position.y());
        rot.Set(aTransformA.angle);
        b2Transform tA(pos, rot);

        pos.Set(aTransformB.position.x(), aTransformB.position.y());
        rot.Set(aTransformB.angle);
        b2Transform tB(pos, rot);

        b2CollideCircles(&mf, static_cast<b2CircleShape*>(aShapeA.mShape), tA,
                         static_cast<b2CircleShape*>(aShapeB.mShape), tB);

        return mf.pointCount != 0;
    }

    return false;
}

PolygonShape Shape::ToPolygonShape() const
{
    return PolygonShape(static_cast<b2PolygonShape*>(mShape));
}

CircleShape Shape::ToCircleShape() const
{
    return CircleShape(static_cast<b2CircleShape*>(mShape));
}
}
}
