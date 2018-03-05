#pragma once

namespace og
{
namespace physics
{
struct ShapeFactory
{
    static b2CircleShape* CreateCircle(float aRadius)
    {
        auto shape = new b2CircleShape;
        shape->m_radius = aRadius;
        return shape;
    }

    static b2PolygonShape* CreateBox(float aWidth, float aHeight)
    {
        auto shape = new b2PolygonShape;
        shape->SetAsBox(aWidth, aHeight);
        return shape;
    }

    static b2EdgeShape* CreateEdge(float aX1, float aY1, float aX2, float aY2)
    {
        b2Vec2 v1(aX1, aY1);
        b2Vec2 v2(aX2, aY2);

        auto shape = new b2EdgeShape;
        shape->Set(v1, v2);
        return shape;
    }

    static b2Shape* CreateShape(const ShapeDef& aDef)
    {
        if (aDef.type == Shape::e_circle)
        {
            return ShapeFactory::CreateCircle(aDef.radius);
        }
        if (aDef.type == Shape::e_polygon)
        {
            return ShapeFactory::CreateBox(aDef.size.width(), aDef.size.height());
        }
        if (aDef.type == Shape::e_edge)
        {
            return ShapeFactory::CreateEdge(aDef.line.x1(), aDef.line.y1(), aDef.line.x2(), aDef.line.y2());
        }

        return nullptr;
    }
};
}
}
