#include "physics.h"
#include "opengoo.h"
#include "wog_scene.h"
#include "wog_material.h"
#include "og_world.h"

#include <OGPhysicsEngine>
#include <OGPhysicsBody>
#include <OGPhysicsJoint>

#include <QLineF>
#include <qmath.h>

using namespace og;
using namespace physics;

OGPhysicsBody* createCircle(const QPointF &position, float radius
                            , float angle, WOGMaterial* material
                            , bool dynamic, float mass, OGUserData* data)
{
    return createCircle(position.x(), position.y(), radius, angle, material
                        , dynamic, mass, data);
}

OGPhysicsBody* createCircle(float x, float y, float radius
                            , float angle, WOGMaterial* material
                            , bool dynamic, float mass, OGUserData* data)
{
    OGPhysicsBody* circle;
    OGPhysicsEngine* engine;
    b2Filter filter;

    engine = OGPhysicsEngine::GetInstance();
    circle = new OGPhysicsBody(x * K, y * K, dynamic, angle);

    engine->CreateBody(circle);
    circle->CreateShape(OGPhysicsBody::CIRCLE);
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

OGPhysicsBody* createLine(const QPointF &anchor, const QPointF &normal
                          , WOGMaterial* material, bool dynamic
                          , OGUserData* data)
{
    float x1, x2, y1, y2, length, angle;
    OGPhysicsBody* line;
    OGPhysicsEngine* engine;

    engine = OGPhysicsEngine::GetInstance();
    x1 = anchor.x() * K * 0.5f;
    y1 = anchor.y() * K * 0.5f;

    //0.55 = (length + 10%)/2

    float wScene = sceneWidth();
    float hScene = sceneHeight();
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

    line = new OGPhysicsBody(x1, y1, dynamic, 0);

    engine->CreateBody(line);
    line->CreateShape(OGPhysicsBody::EDGE);
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

OGPhysicsBody* createRectangle(const QPointF &position, const QSizeF &size
                               , qreal angle, WOGMaterial* material
                               , bool dynamic, float mass, OGUserData* data)
{
    return createRectangle(position.x(), position.y(), size.width()
                           , size.height(), angle, material, dynamic, mass
                           , data);
}

OGPhysicsBody* createRectangle(float x, float y, float width
                               , float height, float angle
                               , WOGMaterial* material
                               , bool dynamic, float mass, OGUserData* data)
{
    OGPhysicsBody* rect;
    OGPhysicsEngine* engine;
    b2Filter filter;

    engine = OGPhysicsEngine::GetInstance();
    rect = new OGPhysicsBody(x * K, y * K , dynamic, angle);

    engine->CreateBody(rect);
    rect->CreateShape(OGPhysicsBody::POLYGON);
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

OGPhysicsJoint* createJoint(OGPhysicsBody* b1, OGPhysicsBody* b2
                            , OGUserData* data)
{
    OGPhysicsJoint* joint;
    OGPhysicsEngine* engine;
    b2DistanceJointDef* jointDef;

    engine = OGPhysicsEngine::GetInstance();
    joint = new OGPhysicsJoint();
    jointDef = new b2DistanceJointDef;
    jointDef->frequencyHz = 1.5f;
    jointDef->dampingRatio = 0.9f;

    jointDef->Initialize(b1->body, b2->body
                         , b1->body->GetPosition(), b2->body->GetPosition());

    joint->jointdef = jointDef;
    engine->CreateJoint(joint);
    joint->joint->SetUserData(data);

    return joint;
}

void setMass(OGPhysicsBody* b, float mass)
{
    b2MassData m;

    b->body->GetMassData(&m);
    m.mass = mass;
    b->body->SetMassData(&m);
}

void setBodyPosition(OGPhysicsBody* b, float x, float y)
{
    b->body->SetTransform(b2Vec2(x, y), b->body->GetAngle());
    b->body->SetAwake(false);
}

WOGScene* physics::GetScene()
{       
    return OpenGOO::instance()->GetWorld()->scenedata();
}

float physics::sceneWidth()
{
    WOGScene* scene = GetScene();

    return scene->maxx - scene->minx;
}

float physics::sceneHeight()
{
    WOGScene* scene = GetScene();

    return scene->maxy - scene->miny;
}
