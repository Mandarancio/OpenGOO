#ifndef PHYSICS_H
#define PHYSICS_H

#include "og_physicsengine.h"
#include "og_world.h"
#include "og_userdata.h"

bool initializePhysicsEngine(const QPointF &gravity, bool sleep);

OGPhysicsBody* createCircle(float32 x, float32 y, float32 radius, float32 angle
                            , WOGMaterial* material, bool dynamic
                            , float mass, OGUserData* data);

OGPhysicsBody* createCircle(const QPointF & position, float32 radius
                            , float32 angle, WOGMaterial* material
                            , bool dynamic=false, float mass=0
        , OGUserData* data=0);

OGPhysicsBody* createLine(const QPointF & anchor, const QPointF & normal
                          , WOGMaterial* material, bool dynamic=false
        , OGUserData* data=0);

OGPhysicsBody* createRectangle(float32 x, float32 y, float32 width
                               , float32 height, float32 angle
                               , WOGMaterial* material, bool dynamic
                               , float mass, OGUserData* data);

OGPhysicsBody* createRectangle(const QPointF & position, const QSizeF & size
                               , qreal angle, WOGMaterial* material
                               , bool dynamic=false, float mass=0
                               , OGUserData* data=0);
                               
OGPhysicsJoint* createJoint(OGPhysicsBody* b1, OGPhysicsBody* b2
                            , OGUserData* data);

void setMass(OGPhysicsBody* b, float mass);
void setBodyPosition(OGPhysicsBody* body, float x, float y);

#endif // PHYSICS_H
