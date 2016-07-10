#pragma once

#include <QPointF>
#include <QSizeF>

#include "Box2D/Common/b2Settings.h"

#include <OGPhysicsBody>
#include <OGPhysicsJoint>

struct WOGMaterial;
struct WOGScene;

struct OGUserData;

namespace og
{
namespace global
{
WOGScene* GetScene();
float sceneWidth();
float sceneHeight();
}
}

//bool initializePhysicsEngine(const QPointF &gravity, bool sleep);

class PhysicsFactory
{
public:
    static float K;

    // categories
    static uint16 LINE;
    static uint16 CIRCLE;
    static uint16 RECTANGLE;
    static uint16 BALL;
    static uint16 EXIT;
    static uint16 SENSOR;
    static uint16 STATIC;

    static og::PhysicsBody* createCircle(float x, float y, float radius, float angle
                                , WOGMaterial* material, bool dynamic
                                , float mass, OGUserData* data);

    static og::PhysicsBody* createCircle(const QPointF &position, float radius
                                , float angle, WOGMaterial* material
                                , bool dynamic = false, float mass = 0
                                        , OGUserData* data = 0);

    static og::PhysicsBody* createLine(const QPointF &anchor, const QPointF &normal
                              , WOGMaterial* material, bool dynamic = false
                                      , OGUserData* data = 0);

    static og::PhysicsBody* createRectangle(float x, float y, float width
                                   , float height, float angle
                                   , WOGMaterial* material, bool dynamic
                                   , float mass, OGUserData* data);

    static og::PhysicsBody* createRectangle(const QPointF& position,
                                              const QSizeF& size,
                                              qreal angle,
                                              WOGMaterial* material, bool dynamic = false,
                                              float mass = 0,
                                              OGUserData* data = 0);

    static og::OGPhysicsJoint* createJoint(og::PhysicsBody* b1,
                                           og::PhysicsBody* b2,
                                           OGUserData* data);
};

void setMass(og::PhysicsBody* b, float mass);
void setBodyPosition(og::PhysicsBody* body, float x, float y);
