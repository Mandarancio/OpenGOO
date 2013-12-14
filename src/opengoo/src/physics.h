#ifndef PHYSICS_H
#define PHYSICS_H

#include <OGPhysicsBody>
#include <OGPhysicsJoint>

#include <QPointF>
#include <QSizeF>

struct WOGMaterial;
struct WOGScene;

struct OGUserData;

#include "Box2D/Common/b2Settings.h"

namespace physics
{
const float K = 0.1f;

// categories
const uint16 LINE = 0x0002;
const uint16 CIRCLE = 0x0004;
const uint16 RECTANGLE = 0x0008;
const uint16 BALL = 0x0010;
const uint16 EXIT = 0x0020;
const uint16 SENSOR = 0x0040;
const uint16 STATIC = LINE | CIRCLE | RECTANGLE;

WOGScene* GetScene();
float sceneWidth();
float sceneHeight();
}

bool initializePhysicsEngine(const QPointF &gravity, bool sleep);

og::OGPhysicsBody* createCircle(float x, float y, float radius, float angle
                            , WOGMaterial* material, bool dynamic
                            , float mass, OGUserData* data);

og::OGPhysicsBody* createCircle(const QPointF &position, float radius
                            , float angle, WOGMaterial* material
                            , bool dynamic = false, float mass = 0
                                    , OGUserData* data = 0);

og::OGPhysicsBody* createLine(const QPointF &anchor, const QPointF &normal
                          , WOGMaterial* material, bool dynamic = false
                                  , OGUserData* data = 0);

og::OGPhysicsBody* createRectangle(float x, float y, float width
                               , float height, float angle
                               , WOGMaterial* material, bool dynamic
                               , float mass, OGUserData* data);

og::OGPhysicsBody* createRectangle(const QPointF &position, const QSizeF &size
                               , qreal angle, WOGMaterial* material
                               , bool dynamic = false, float mass = 0
                                       , OGUserData* data = 0);

og::OGPhysicsJoint* createJoint(og::OGPhysicsBody* b1, og::OGPhysicsBody* b2
                            , OGUserData* data);

void setMass(og::OGPhysicsBody* b, float mass);
void setBodyPosition(og::OGPhysicsBody* body, float x, float y);

#endif // PHYSICS_H
