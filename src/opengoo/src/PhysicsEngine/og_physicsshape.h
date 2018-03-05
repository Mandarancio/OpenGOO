#pragma once

#include <QVector2D>
#include <QLineF>

#include "common.h"

#include "OGLib/size.h"

namespace og
{
namespace physics
{
class Shape
{
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
        e_chain
    };

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

    virtual ~Shape()
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

protected:
    const b2Shape* GetShape() const
    {
        return mShape;
    }

    b2Shape* GetShape()
    {
        return mShape;
    }

private:
    Type m_type;
    b2Shape* mShape;
};

struct ShapeDef
{
    typedef Shape::Type Type;
    typedef oglib::SizeF Size;
    typedef QLineF Line;

    Type type;

    union
    {
        float radius;
        Size size;
        Line line;
    };

    ShapeDef()
        : type(Shape::e_unknown)
    {
    }
};
}
}
