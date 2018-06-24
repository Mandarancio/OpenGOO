#pragma once

namespace og
{
namespace physics
{
struct ShapeFactory
{
    static std::unique_ptr<b2CircleShape> CreateCircle(const CircleShapeDef* aDef)
    {
        std::unique_ptr<b2CircleShape> shape(new b2CircleShape);
        shape->m_radius = aDef->radius;
        shape->m_p.Set(aDef->position.x(), aDef->position.y());
        return shape;
    }

    static std::unique_ptr<b2CircleShape> CreateCircle(float aRadius)
    {
        CircleShapeDef def;
        def.radius = aRadius;
        return CreateCircle(&def);
    }

    static std::unique_ptr<b2PolygonShape> CreateBox(const BoxShapeDef* aDef)
    {
        std::unique_ptr<b2PolygonShape> shape(new b2PolygonShape);
        b2Vec2 center(aDef->center.x(), aDef->center.y());
        shape->SetAsBox(aDef->size.width(), aDef->size.height(), center, aDef->angle);
        return shape;
    }

    static std::unique_ptr<b2PolygonShape> CreateBox(float aWidth, float aHeight)
    {
        std::unique_ptr<b2PolygonShape> shape(new b2PolygonShape);
        shape->SetAsBox(aWidth, aHeight);
        return shape;
    }

    static std::unique_ptr<b2EdgeShape> CreateLine(const LineShapeDef* aDef)
    {
        return CreateLine(aDef->x1, aDef->y1, aDef->x2, aDef->y2);
    }

    static std::unique_ptr<b2EdgeShape> CreateLine(float aX1, float aY1, float aX2, float aY2)
    {
        b2Vec2 v1(aX1, aY1);
        b2Vec2 v2(aX2, aY2);

        std::unique_ptr<b2EdgeShape> shape(new b2EdgeShape);
        shape->Set(v1, v2);
        return shape;
    }

    static std::unique_ptr<b2Shape> CreateShape(const ShapeDef* aDef)
    {
        if (aDef->GetType() == Shape::e_circle)
        {
            return ShapeFactory::CreateCircle(static_cast<const CircleShapeDef*>(aDef));
        }
        if (aDef->GetType() == Shape::e_box)
        {
            return ShapeFactory::CreateBox(static_cast<const BoxShapeDef*>(aDef));
        }
        if (aDef->GetType() == Shape::e_line)
        {
            return ShapeFactory::CreateLine(static_cast<const LineShapeDef*>(aDef));
        }

        return nullptr;
    }
};
}
}
