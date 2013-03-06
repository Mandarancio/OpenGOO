#include "physics.h"

#include <qmath.h>
#include <QDebug>

namespace physics
{
    const qreal K = 0.1;

    // categories
    const uint16 LINE = 0x0002;
    const uint16 CIRCLE = 0x0004;
    const uint16 RECTANGLE = 0x0008;
    const uint16 BALL = 0x0010;
    const uint16 STATIC = LINE | CIRCLE | RECTANGLE;
}

extern OGWorld* _world;

using namespace physics;

OGPhysicsBody* createCircle(const QPointF & position, float32 radius
                            , float32 angle, WOGMaterial* material
                            , bool dynamic, float mass, OGUserData* data)
{
    return createCircle(position.x(), position.y(), radius, angle, material
                        , dynamic, mass, data);
}

OGPhysicsBody* createCircle(float32 x, float32 y, float32 radius
                            , float32 angle, WOGMaterial* material
                            , bool dynamic, float mass, OGUserData *data)
{
    OGPhysicsBody* circle;
    OGPhysicsEngine* engine;
    b2Filter filter;

    engine = OGPhysicsEngine::GetInstance();
    circle = new OGPhysicsBody(x*K, y*K, dynamic, angle);

    engine->CreateBody(circle);
    circle->CreateShape(OGPhysicsBody::CIRCLE);
    circle->shape->SetPosition(0.0, 0.0);
    circle->shape->SetRadius(radius*K);

    if (dynamic)
    {
        float32 density = (mass*K)/(M_PI*radius*radius*K*K);

        circle->CreateFixture(density, material->friction*0.01
                              , material->bounce);

        filter.categoryBits = BALL;
        filter.maskBits = STATIC;
        circle->fixture->SetFilterData(filter);
        circle->body->SetUserData(data);
    }
    else
    {
        circle->CreateFixture(0.0, material->friction*0.01, material->bounce);

        filter.categoryBits = CIRCLE;
        filter.maskBits = BALL;
        circle->fixture->SetFilterData(filter);
        circle->body->SetUserData(data);
    }

    return circle;
}

OGPhysicsBody* createLine(const QPointF & anchor, const QPointF & normal
                          , WOGMaterial* material, bool dynamic
                          , OGUserData* data)
{
    qreal x1, x2, y1, y2, length, angle;
    OGPhysicsBody* line;
    OGPhysicsEngine* engine;

    engine = OGPhysicsEngine::GetInstance();
    x1 = anchor.x()*K*0.5;
    y1 = anchor.y()*K*0.5;

    //0.55 = (length + 10%)/2
    length = qMax(_world->scenesize().width(), _world->scenesize().height())
            *0.55*K;

    x2 = x1 + normal.x();
    y2 = y1 + normal.y();

    QLineF line1(x1, y1, x2, y2);
    QLineF line2(line1.normalVector());

    line2.setLength(length);

    QLineF line3(line2);

    angle = line3.angle() + 180.0;
    line3.setAngle(angle);

    QPointF p1(line3.p2());
    QPointF p2(line2.p2());

    line = new OGPhysicsBody(x1, y1, dynamic, 0);

    engine->CreateBody(line);
    line->CreateShape(OGPhysicsBody::EDGE);
    line->shape->Set(p1.x(), p1.y(), p2.x(), p2.y());

    if (dynamic)
    {
        line->CreateFixture(0.0, material->friction*0.01, material->bounce);
    }
    else
    {
        line->CreateFixture(0.0, material->friction*0.01, material->bounce);

        b2Filter filter;

        filter.categoryBits = LINE;
        filter.maskBits = BALL;
        line->fixture->SetFilterData(filter);
        line->body->SetUserData(data);
    }

    return line;
}

OGPhysicsBody* createRectangle(const QPointF & position, const QSizeF & size
                               , qreal angle, WOGMaterial* material
                               , bool dynamic, float mass, OGUserData* data)
{
    return createRectangle(position.x(), position.y(), size.width()
                           , size.height(), angle, material, dynamic, mass
                           , data);
}

OGPhysicsBody* createRectangle(float32 x, float32 y, float32 width
                               , float32 height, float32 angle
                               , WOGMaterial* material
                               , bool dynamic, float mass, OGUserData* data)
{
    OGPhysicsBody* rect;
    OGPhysicsEngine* engine;
    b2Filter filter;

    engine = OGPhysicsEngine::GetInstance();
    rect = new OGPhysicsBody(x*K, y*K , dynamic, angle);

    engine->CreateBody(rect);
    rect->CreateShape(OGPhysicsBody::POLYGON);
    rect->shape->SetAsBox(width*K*0.5, height*K*0.5);

    if (dynamic)
    {
        float32 density = (mass*K)/(width*height*K*K);

        rect->CreateFixture(density, material->friction*0.01
                              , material->bounce);

        rect->CreateFixture(density, material->friction*0.01
                            , material->bounce);

        filter.categoryBits = BALL;
        filter.maskBits = STATIC;
        rect->fixture->SetFilterData(filter);
        rect->body->SetUserData(data);
    }
    else
    {
        rect->CreateFixture(0.0, material->friction*0.01, material->bounce);

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
    jointDef->frequencyHz = 1.5;
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
