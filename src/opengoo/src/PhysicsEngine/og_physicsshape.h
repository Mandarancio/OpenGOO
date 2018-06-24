#pragma once

#include <QVector2D>
#include <QLineF>

#include "common.h"

#include "OGLib/size.h"

namespace og
{
class PhysicsBody;

namespace physics
{
class PolygonShape;
class CircleShape;

class Shape
{
    friend class og::PhysicsBody;

public:
    struct Transform
    {
        QVector2D position;
        float angle;
    };

public:
    enum Type
    {
        e_unknown,
        e_circle,
        e_polygon,
        e_edge,
        e_chain,
        e_box,
        e_line
    };

    Shape()
        : mShape(nullptr)
        , m_type(e_unknown)
    {
    }

    float GetRadius() const
    {
        return mShape->m_radius;
    }

    Type GetType() const
    {
        return m_type;
    }

    bool TestPoint(const Transform& a_transform, const QVector2D& a_point) const;

    static bool Collide(const Shape& aShapeA, const Transform& aTransformA, const Shape& aShapeB, const Transform& aTransformB);

    PolygonShape ToPolygonShape() const;

    CircleShape ToCircleShape() const;

protected:
    Shape(b2Shape* aShape)
        : mShape(aShape)
    {
        if (aShape->GetType() == b2Shape::e_circle)
        {
            m_type = e_circle;
        }
        else if (aShape->GetType() == b2Shape::e_polygon)
        {
            m_type = e_polygon;
        }
        else if (aShape->GetType() == b2Shape::e_edge)
        {
            m_type = e_edge;
        }
        else
        {
            assert(false);
            m_type = e_unknown;
        }
    }

    b2Shape* GetShape() const
    {
        return mShape;
    }

protected:
    b2Shape* mShape;
    Type m_type;
};

struct ShapeDef
{
    typedef Shape::Type Type;
    typedef oglib::SizeF Size;
    typedef QLineF Line;

    ShapeDef()
        : mType(Shape::e_unknown)
    {
    }

    Type GetType() const
    {
        return mType;
    }

protected:
    Type mType;
};

struct CircleShapeDef : public ShapeDef
{
    CircleShapeDef()
        : radius(0)
    {
        mType = Shape::e_circle;
    }

    float radius;
    QVector2D position;
};

struct BoxShapeDef : public ShapeDef
{
    BoxShapeDef()
        : angle(0)
    {
        mType = Shape::e_box;
    }

    Size size;
    QVector2D center;
    float angle;
};

struct LineShapeDef : public ShapeDef
{
    LineShapeDef()
    {
        mType = Shape::e_line;
        x1 = y1 = x2 = y2 = 0;
    }

    float x1, y1, x2, y2;
};
}
}
