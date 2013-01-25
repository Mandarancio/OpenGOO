#ifndef PHYSICS_H
#define PHYSICS_H

#include "og_physicsengine.h"
#include "og_world.h"

bool initializePhysicsEngine(const QPointF gravity, bool sleep);

OGPhysicsBody* createCircle(const QPointF & position, float32 radius
                            , const WOGMaterial & material, bool dynamic
                            , qreal mass
                            );

OGPhysicsBody* createLine(const QPointF & anchor, const QPointF & normal
                          , const WOGMaterial & material, OGWorld* world
                          , bool dynamic
                          );

OGPhysicsBody* createRectangle(const QPointF & position, const QSizeF & size
                               , qreal rotation, const WOGMaterial & material
                               , bool dynamic
                               );

#endif // PHYSICS_H
