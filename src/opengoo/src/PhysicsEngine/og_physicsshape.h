#ifndef OG_PHYSICSSHAPE_H
#define OG_PHYSICSSHAPE_H

#include "common.h"

struct OGPhysicsShape
{
    b2Shape* shape;

    OGPhysicsShape(b2Shape::Type type);
    virtual ~OGPhysicsShape() { delete shape; }

    b2Vec2 GetP1() const; // Get the line's start point
    b2Vec2 GetP2() const; // Get the line's end point
    float32 GetRadius() const { return shape->m_radius; }
    void SetAsBox(float32 width, float32 height); // Create the box
    void SetAsBox(float32 width, float32 height, const b2Vec2 & center
                  , float32 angle
                  );

    void SetPosition(float32 x, float32 y);
    void SetRadius(float32 radius) { shape->m_radius = radius; }
    void Set(float32 x1, float32 y1, float32 x2, float y2);
};

#endif // OG_PHYSICSSHAPE_H
