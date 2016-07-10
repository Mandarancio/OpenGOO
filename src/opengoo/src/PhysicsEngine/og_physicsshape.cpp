#include "og_physicsshape.h"

namespace og
{
namespace physics
{
Shape::Shape(Type a_type)
{
    m_type = a_type;

    switch(a_type)
    {
    case e_circle:    
        m_shape.reset(new b2CircleShape);
        break;
    case e_polygon:
        m_shape.reset(new b2PolygonShape);
        break;                
    case e_edge:
        m_shape.reset(new b2EdgeShape);
        break;
    case e_chain:
        m_shape.reset(new b2ChainShape);
        break;
    default:
        break;
    }
}

void Shape::SetAsBox(float32 width, float32 height)
{
    if (GetType() == e_polygon)
    {
        auto rect = static_cast<b2PolygonShape*>(m_shape.get());
        rect->SetAsBox(width, height);
    }
}

void Shape::SetAsBox(float32 width,
                     float32 height,
                     const b2Vec2& center,
                     float32 angle)
{
    if (GetType() == e_polygon)
    {
        auto rect = static_cast<b2PolygonShape*>(m_shape.get());
        rect->SetAsBox(width, height, center, angle);
    }
}

void Shape::SetPosition(float32 x, float32 y)
{
    if (GetType() == e_circle)
    {
        auto circle = static_cast<b2CircleShape*>(m_shape.get());
        circle->m_p.Set(x, y);
    }
}

void Shape::SetPosition(const QPointF& pos)
{
    SetPosition(pos.x(), pos.y());
}

void Shape::Set(float32 x1, float32 y1, float32 x2, float y2)
{
    if (GetType() == e_edge)
    {
        auto line = static_cast<b2EdgeShape*>(m_shape.get());
        line->Set(b2Vec2(x1, y1), b2Vec2(x2, y2));
    }
}

b2Vec2 Shape::GetP1() const
{
    if (GetType() == e_edge)
    {
        auto line = static_cast<b2EdgeShape*>(m_shape.get());
        return line->m_vertex1;
    }

    return b2Vec2();
}

b2Vec2 Shape::GetP2() const
{
    if (GetType() == e_edge)
    {
        auto line = static_cast<b2EdgeShape*>(m_shape.get());
        return line->m_vertex2;
    }

    return b2Vec2();
}
}
}
