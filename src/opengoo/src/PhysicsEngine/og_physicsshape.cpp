#include "og_physicsshape.h"

OGPhysicsShape::OGPhysicsShape(b2Shape::Type type)
{
    switch(type)
    {
    case b2Shape::e_circle:
        shape = new b2CircleShape;
        break;

    case b2Shape::e_polygon:
        shape = new b2PolygonShape;
        break;                

    case b2Shape::e_edge:
        shape = new b2EdgeShape;
        break;

    case b2Shape::e_chain:
        shape = new b2ChainShape;
        break;
    default:
        break;
    }
}

void OGPhysicsShape::SetAsBox(float32 width, float32 height)
{
    switch(shape->GetType())
    {
    case b2Shape::e_polygon:
    {
        b2PolygonShape* rect = static_cast<b2PolygonShape*>(shape);
        rect->SetAsBox(width, height);
    }
        break;
    default:
        break;
    }
}

void OGPhysicsShape::SetAsBox(float32 width, float32 height
                              , const b2Vec2 & center, float32 angle
                              )
{
    switch(shape->GetType())
    {
    case b2Shape::e_polygon:
    {
        b2PolygonShape* rect = static_cast<b2PolygonShape*>(shape);
        rect->SetAsBox(width, height, center, angle);
    }
        break;
    default:
        break;
    }
}

void OGPhysicsShape::SetPosition(float32 x, float32 y)
{
    switch(shape->GetType())
    {
    case b2Shape::e_circle:
    {
        b2CircleShape* circle = static_cast<b2CircleShape*>(shape);
        circle->m_p.Set(x, y);
    }
        break;
    default:
        break;
    }
}

void OGPhysicsShape::SetPosition(const QPointF& pos)
{
    SetPosition(pos.x(), pos.y());
}

void OGPhysicsShape::Set(float32 x1, float32 y1, float32 x2, float y2)
{
    switch(shape->GetType())
    {
    case b2Shape::e_edge:
    {
        b2EdgeShape* line = static_cast<b2EdgeShape*>(shape);
        line->Set(b2Vec2(x1, y1), b2Vec2(x2, y2));
    }
        break;
    default:
        break;
    }
}

b2Vec2 OGPhysicsShape::GetP1() const
{
    switch(shape->GetType())
    {
    case b2Shape::e_edge:
    {
        b2EdgeShape* line = static_cast<b2EdgeShape*>(shape);
        return line->m_vertex1;
    }
    default:
        break;
    }

    return b2Vec2();
}

b2Vec2 OGPhysicsShape::GetP2() const
{
    switch(shape->GetType())
    {
    case b2Shape::e_edge:
    {
        b2EdgeShape* line = static_cast<b2EdgeShape*>(shape);
        return line->m_vertex2;
    }
    default:
        break;
    }

    return b2Vec2();
}
