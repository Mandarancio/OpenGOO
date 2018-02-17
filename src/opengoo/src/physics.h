#pragma once

#include <QPointF>
#include <QSizeF>

#include "Box2D/Common/b2Settings.h"

#include <OGPhysicsBody>
#include <OGPhysicsJoint>

struct WOGMaterial;
struct WOGScene;

struct OGUserData;

class PhysicsFactory
{
public:
    static float MetersToPixels;
    static float PixelsToMeters;

    // categories
    static uint16 LINE;
    static uint16 CIRCLE;
    static uint16 RECTANGLE;
    static uint16 BALL;
    static uint16 EXIT;
    static uint16 SENSOR;
    static uint16 STATIC;

    static og::PhysicsBody* createCircle(og::physics::PhysicsEngine& a_physicEngine,
                                         float x,
                                         float y,
                                         float radius,
                                         float angle,
                                         const WOGMaterial& material,
                                         bool dynamic,
                                         float mass,
                                         OGUserData* data);

    static og::PhysicsBody* createCircle(og::physics::PhysicsEngine& a_physicEngine,
                                         const QPointF &position,
                                         float radius,
                                         float angle,
                                         const WOGMaterial& material,
                                         bool dynamic = false,
                                         float mass = 0.0f,
                                         OGUserData* data = nullptr);

    static og::PhysicsBody* createLine(og::physics::PhysicsEngine& a_physicEngine,
                                       const QPointF &anchor, const QPointF &normal,
                                       const WOGMaterial& material,
                                       bool dynamic = false,
                                       OGUserData* data = nullptr);

    static og::PhysicsBody* createRectangle(og::physics::PhysicsEngine& a_physicEngine,
                                            float x,
                                            float y,
                                            float width,
                                            float height,
                                            float angle,
                                            const WOGMaterial& material,
                                            bool dynamic,
                                            float mass,
                                            OGUserData* data);

    static og::PhysicsBody* createRectangle(og::physics::PhysicsEngine& a_physicEngine,
                                            const QPointF& position,
                                            const QSizeF& size,
                                            float angle,
                                            const WOGMaterial& material,
                                            bool dynamic = false,
                                            float mass = 0.0f,
                                            OGUserData* data = nullptr);

    static og::physics::Joint* createJoint(og::physics::PhysicsEngine& a_physicEngine,
                                           og::PhysicsBody* b1,
                                           og::PhysicsBody* b2,
                                           OGUserData* data);
};

void setMass(og::PhysicsBody* b, float mass);
void setBodyPosition(og::PhysicsBody* body, float x, float y);
