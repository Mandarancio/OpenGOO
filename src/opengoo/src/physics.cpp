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


float PhysicsFactory::K = 0.1f;
uint16 PhysicsFactory::LINE = 0x0002;
uint16 PhysicsFactory::CIRCLE = 0x0004;
uint16 PhysicsFactory::RECTANGLE = 0x0008;
uint16 PhysicsFactory::BALL = 0x0010;
uint16 PhysicsFactory::EXIT = 0x0020;
uint16 PhysicsFactory::SENSOR = 0x0040;
uint16 PhysicsFactory::STATIC = LINE | CIRCLE | RECTANGLE;

og::PhysicsBody* PhysicsFactory::createCircle(const QPointF &position,
                                            float radius,
                                            float angle,
                                            WOGMaterial* material,
                                            bool dynamic,
                                            float mass,
                                            OGUserData* data)
{
    return createCircle(position.x(), position.y(), radius, angle, material
                        , dynamic, mass, data);
}

og::PhysicsBody* PhysicsFactory::createCircle(float x,
                                            float y,
                                            float radius,
                                            float angle,
                                            WOGMaterial* material,
                                            bool dynamic,
                                            float mass,
                                            OGUserData* data)
{
    b2Filter filter;

    auto circle = new og::PhysicsBody(x * K, y * K, dynamic, angle);

    PE->CreateBody(circle);
    circle->CreateShape(og::physics::Shape::e_circle);
    circle->shape->SetPosition(0.0f, 0.0f);
    circle->shape->SetRadius(radius * K);

    if (dynamic)
    {
        float density = (mass * K) / (M_PI * radius * radius * K * K);

        circle->CreateFixture(density, material->friction * 0.01f
                              , material->bounce);

        filter.categoryBits = BALL;
        filter.maskBits = STATIC | EXIT | SENSOR;
        circle->fixture->SetFilterData(filter);
        circle->body->SetUserData(data);
    }
    else
    {
        circle->CreateFixture(0.0f, material->friction * 0.01f
                              , material->bounce);

        filter.categoryBits = CIRCLE;
        filter.maskBits = BALL;
        circle->fixture->SetFilterData(filter);
        circle->body->SetUserData(data);
    }

    return circle;
}

og::PhysicsBody* PhysicsFactory::createLine(const QPointF &anchor,
                          const QPointF &normal,
                          WOGMaterial* material,
                          bool dynamic,
                          OGUserData* data)
{
    float x1, x2, y1, y2, length, angle;

    x1 = anchor.x() * K * 0.5f;
    y1 = anchor.y() * K * 0.5f;

    //0.55 = (length + 10%)/2

    float wScene = og::global::sceneWidth();
    float hScene = og::global::sceneHeight();
    length = qMax(wScene, hScene) * 0.55f * K;

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

    PE->CreateBody(line);
    line->CreateShape(og::physics::Shape::e_edge);
    line->shape->Set(p1.x(), p1.y(), p2.x(), p2.y());

    if (dynamic)
    {
        line->CreateFixture(0.0f, material->friction * 0.01f
                            , material->bounce);
    }
    else
    {
        line->CreateFixture(0.0f, material->friction * 0.01f
                            , material->bounce);

        b2Filter filter;

        filter.categoryBits = LINE;
        filter.maskBits = BALL;
        line->fixture->SetFilterData(filter);
        line->body->SetUserData(data);
    }

    return line;
}

og::PhysicsBody* PhysicsFactory::createRectangle(const QPointF &position,
                                               const QSizeF &size,
                                               qreal angle,
                                               WOGMaterial* material,
                                               bool dynamic,
                                               float mass,
                                               OGUserData* data)
{
    return createRectangle(position.x(), position.y(), size.width()
                           , size.height(), angle, material, dynamic, mass
                           , data);
}

og::PhysicsBody* PhysicsFactory::createRectangle(float x,
                               float y,
                               float width,
                               float height,
                               float angle,
                               WOGMaterial* material,
                               bool dynamic,
                               float mass,
                               OGUserData* data)
{
    b2Filter filter;

    auto rect = new og::PhysicsBody(x * K, y * K , dynamic, angle);

    PE->CreateBody(rect);
    rect->CreateShape(og::physics::Shape::e_polygon);
    rect->shape->SetAsBox(width * K * 0.5f, height * K * 0.5f);

    if (dynamic)
    {
        float density = (mass * K) / (width * height * K * K);

        rect->CreateFixture(density, material->friction * 0.01f
                            , material->bounce);

        rect->CreateFixture(density, material->friction * 0.01f
                            , material->bounce);

        filter.categoryBits = BALL;
        filter.maskBits = STATIC;
        rect->fixture->SetFilterData(filter);
        rect->body->SetUserData(data);
    }
    else
    {
        rect->CreateFixture(0.0f, material->friction * 0.01f
                            , material->bounce);

        filter.categoryBits = RECTANGLE;
        filter.maskBits = BALL;
        rect->fixture->SetFilterData(filter);
        rect->body->SetUserData(data);
    }

    return rect;
}

og::OGPhysicsJoint* PhysicsFactory::createJoint(og::PhysicsBody* b1,
                                                og::PhysicsBody* b2,
                                                OGUserData* data)
{
    auto joint = new og::OGPhysicsJoint;
    auto jointDef = new b2DistanceJointDef;
    jointDef->frequencyHz = 1.5f;
    jointDef->dampingRatio = 0.9f;

    jointDef->Initialize(b1->body, b2->body
                         , b1->body->GetPosition(), b2->body->GetPosition());

    joint->jointdef = jointDef;
    PE->CreateJoint(joint);
    joint->joint->SetUserData(data);

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

namespace og
{
namespace global
{
WOGScene* GetScene()
{       
    return OpenGOO::GetInstance()->GetWorld()->scenedata();
}

float sceneWidth()
{
    WOGScene* scene = GetScene();

    return scene->maxx - scene->minx;
}

float sceneHeight()
{
    WOGScene* scene = GetScene();

    return scene->maxy - scene->miny;
}
}
}
