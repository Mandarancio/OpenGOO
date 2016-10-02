#pragma once

#include <QVector2D>

#include "common.h"

namespace og
{
namespace physics
{
class Shape
{
    typedef std::unique_ptr<b2Shape> ShapeUPtr;

public:
    struct Transform
    {
        QVector2D position;
        float angle;
    };

public:
    enum Type
    {
        e_circle,
        e_polygon,
        e_edge,
        e_chain
    };

    Shape(Type a_type);

    virtual ~Shape()
    {
    }

    b2Vec2 GetP1() const; // Get the line's start point
    b2Vec2 GetP2() const; // Get the line's end point

    float32 GetRadius() const
    {
        return m_shape->m_radius;
    }

    void SetAsBox(float32 width, float32 height); // Create the box
    void SetAsBox(float32 width, float32 height, const b2Vec2 &center
                  , float32 angle);

    void SetPosition(float32 x, float32 y);
    void SetPosition(const QPointF &pos);

    void SetRadius(float32 radius)
    {
        m_shape->m_radius = radius;
    }

    void Set(float32 x1, float32 y1, float32 x2, float y2);

    b2Shape* GetShape() const
    {
        return m_shape.get();
    }

    Type GetType() const
    {
        return m_type;
    }

    bool IsPolygon() const
    {
        return (GetType() == e_polygon);
    }

    bool TestPoint(const Transform& a_transform, const QVector2D& a_point) const;

private:
    ShapeUPtr m_shape;
    Type m_type;
};
}
}
