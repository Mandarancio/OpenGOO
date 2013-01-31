#ifndef PHYSICS_H
#define PHYSICS_H

#include "og_physicsengine.h"
#include "og_world.h"

bool initializePhysicsEngine(const QPointF gravity, bool sleep);

OGPhysicsBody* createCircle(const QPointF & position, float32 radius
                            , WOGMaterial* material, bool dynamic=false
                            , qreal mass=0
                            );

OGPhysicsBody* createLine(const QPointF & anchor, const QPointF & normal
                          , WOGMaterial* material, OGWorld* world
                          , bool dynamic=false
                          );

OGPhysicsBody* createRectangle(const QPointF & position, const QSizeF & size
                               , qreal rotation, WOGMaterial* material
                               , bool dynamic=false
                               );

#endif // PHYSICS_H
