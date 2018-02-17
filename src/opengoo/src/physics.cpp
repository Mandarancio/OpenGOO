#include <QLineF>
#include <qmath.h>

#include "physics.h"
#include "opengoo.h"
#include "wog_scene.h"
#include "wog_material.h"
#include "og_world.h"

#include <OGPhysicsEngine>
#include <OGPhysicsBody>
#include <OGPhysicsJoint>
#include "PhysicsEngine/distancejoint.h"


float PhysicsFactory::MetersToPixels = 32.0f;
float PhysicsFactory::PixelsToMeters = 1.0f / PhysicsFactory::MetersToPixels;
uint16 PhysicsFactory::LINE = 0x0002;
uint16 PhysicsFactory::CIRCLE = 0x0004;
uint16 PhysicsFactory::RECTANGLE = 0x0008;
uint16 PhysicsFactory::BALL = 0x0010;
uint16 PhysicsFactory::EXIT = 0x0020;
uint16 PhysicsFactory::SENSOR = 0x0040;
uint16 PhysicsFactory::STATIC = LINE | CIRCLE | RECTANGLE;

og::PhysicsBody* PhysicsFactory::createCircle(og::physics::PhysicsEngine& a_physicEngine,
                                              const QPointF &position,
                                              float radius,
                                              float angle,
                                              const WOGMaterial& material,
                                              bool dynamic,
                                              float mass,
                                              OGUserData* data)
{
    return createCircle(a_physicEngine,
                        position.x(),
                        position.y(),
                        radius,
                        angle,
                        material,
                        dynamic,
                        mass,
                        data);
}

og::PhysicsBody* PhysicsFactory::createCircle(og::physics::PhysicsEngine& a_physicEngine,
                                              float x,
                                            float y,
                                            float radius,
                                            float angle,
                                            const WOGMaterial& material,
                                            bool dynamic,
                                            float mass,
                                            OGUserData* data)
{
    b2Filter filter;

    auto circle = new og::PhysicsBody(x * PixelsToMeters, y * PixelsToMeters, dynamic, angle);

    a_physicEngine.CreateBody(circle);
    circle->CreateShape(og::physics::Shape::e_circle);
    circle->shape->SetPosition(0.0f, 0.0f);
    circle->shape->SetRadius(radius * PixelsToMeters);

    if (dynamic)
    {
        auto r = radius * PixelsToMeters;
        float density = (mass * PixelsToMeters) / (M_PI * r * r);

        circle->CreateFixture(density, material.friction * 0.01f, material.bounce);

        filter.categoryBits = BALL;
        filter.maskBits = STATIC | EXIT | SENSOR;
        circle->fixture->SetFilterData(filter);
        circle->body->SetUserData(data);
    }
    else
    {
        circle->CreateFixture(0.0f, material.friction * 0.01f, material.bounce);

        filter.categoryBits = CIRCLE;
        filter.maskBits = BALL;
        circle->fixture->SetFilterData(filter);
        circle->body->SetUserData(data);
    }

    return circle;
}

og::PhysicsBody* PhysicsFactory::createLine(og::physics::PhysicsEngine& a_physicEngine,
                                            const QPointF &anchor,
                                              const QPointF &normal,
                                              const WOGMaterial& material,
                                              bool dynamic,
                                              OGUserData* data)
{
    float x1, x2, y1, y2, length, angle;

    x1 = anchor.x() * PixelsToMeters * 0.5f;
    y1 = anchor.y() * PixelsToMeters * 0.5f;

    //0.55 = (length + 10%)/2

    float wScene = GAME->GetScene()->GetWidth();
    float hScene = GAME->GetScene()->GetHeight();
    length = qMax(wScene, hScene) * 0.55f * PixelsToMeters;

    x2 = x1 + normal.x();
    y2 = y1 + normal.y();

    QLineF line1(x1, y1, x2, y2);
    QLineF line2(line1.normalVector());

    line2.setLength(length);

    QLineF line3(line2);

    angle = line3.angle() + 180.0f;
    line3.setAngle(angle);

    QPointF p1(line3.p2());
    QPointF p2(line2.p2());

    auto line = new og::PhysicsBody(x1, y1, dynamic, 0);

    a_physicEngine.CreateBody(line);
    line->CreateShape(og::physics::Shape::e_edge);
    line->shape->Set(p1.x(), p1.y(), p2.x(), p2.y());

    if (dynamic)
    {
        line->CreateFixture(0.0f, material.friction * 0.01f, material.bounce);
    }
    else
    {
        line->CreateFixture(0.0f, material.friction * 0.01f, material.bounce);

        b2Filter filter;

        filter.categoryBits = LINE;
        filter.maskBits = BALL;
        line->fixture->SetFilterData(filter);
        line->body->SetUserData(data);
    }

    return line;
}

og::PhysicsBody* PhysicsFactory::createRectangle(og::physics::PhysicsEngine& a_physicEngine,
                                                 const QPointF &position,
                                                 const QSizeF &size,
                                                 float angle,
                                                 const WOGMaterial& material,
                                                 bool dynamic,
                                                 float mass,
                                                 OGUserData* data)
{
    return createRectangle(a_physicEngine,
                           position.x(),
                           position.y(),
                           size.width(),
                           size.height(),
                           angle,
                           material,
                           dynamic,
                           mass,
                           data);
}

og::PhysicsBody* PhysicsFactory::createRectangle(og::physics::PhysicsEngine& a_physicEngine,
                                                 float x,
                                                 float y,
                                                 float width,
                                                 float height,
                                                 float angle,
                                                 const WOGMaterial& material,
                                                 bool dynamic,
                                                 float mass,
                                                 OGUserData* data)
{
    b2Filter filter;

    auto rect = new og::PhysicsBody(x * PixelsToMeters, y * PixelsToMeters , dynamic, angle);

    a_physicEngine.CreateBody(rect);
    rect->CreateShape(og::physics::Shape::e_polygon);
    rect->shape->SetAsBox(width * PixelsToMeters * 0.5f, height * PixelsToMeters * 0.5f);

    if (dynamic)
    {
        float density = (mass * PixelsToMeters) / (width * height * PixelsToMeters * PixelsToMeters);

        rect->CreateFixture(density, material.friction * 0.01f, material.bounce);

        filter.categoryBits = BALL;
        filter.maskBits = STATIC;
        rect->fixture->SetFilterData(filter);
        rect->body->SetUserData(data);
    }
    else
    {
        rect->CreateFixture(0.0f, material.friction * 0.01f, material.bounce);

        filter.categoryBits = RECTANGLE;
        filter.maskBits = BALL;
        rect->fixture->SetFilterData(filter);
        rect->body->SetUserData(data);
    }

    return rect;
}

og::physics::Joint* PhysicsFactory::createJoint(og::physics::PhysicsEngine& a_physicEngine,
                                                og::PhysicsBody* b1,
                                                og::PhysicsBody* b2,
                                                OGUserData* data)
{
    auto joint = new og::physics::DistanceJoint(a_physicEngine, b1, b2, data);
    //TODO add frequencyHz & dampingRatio
//    jointDef->frequencyHz = 1.5f;
//    jointDef->dampingRatio = 0.9f;

    return joint;
}

void setMass(og::PhysicsBody* b, float mass)
{
    b2MassData m;

    b->body->GetMassData(&m);
    m.mass = mass;
    b->body->SetMassData(&m);
}

void setBodyPosition(og::PhysicsBody* b, float x, float y)
{
    b2Vec2 pos(x, y);
    b->body->SetTransform(pos, b->body->GetAngle());
    b->body->SetAwake(false);
}
