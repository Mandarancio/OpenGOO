#include "physics.h"

#include <QDebug>

namespace physics
{
    const qreal K = 0.1;
    bool status;
}


using namespace physics;

bool initializePhysicsEngine(const QPointF gravity, bool sleep)
{
    OGPhysicsEngine* engine;

    engine = OGPhysicsEngine::GetInstance();
    engine->SetGravity(gravity.x(), gravity.y());
    engine->SetSleep(sleep);

    return engine->Initialize();
}

OGPhysicsBody* createCircle(const QPointF & position, float32 radius
                            , const WOGMaterial & material, bool dynamic
                            , qreal mass
                            )
{
    OGPhysicsBody* circle;
    OGPhysicsEngine* engine;

    engine = OGPhysicsEngine::GetInstance();

    circle = new OGPhysicsBody(position.x()*K, position.y()*K, dynamic);

    engine->CreateBody(circle);
    circle->CreateShape(OGPhysicsBody::CIRCLE);
    circle->shape->SetPosition(0.0, 0.0);
    circle->shape->SetRadius(radius*K);

    if (dynamic)
    {
        circle->CreateFixture(0.0, material.friction*0.01, material.bounce);
        b2MassData m = {
            mass
            , b2Vec2(0,0)
            , 0.0
        };

        circle->body->SetMassData(&m);
    }
    else
    {
        circle->CreateFixture(0.0, material.friction*0.01, material.bounce);
    }

    return circle;
}

OGPhysicsBody* createLine(const QPointF & anchor, const QPointF & normal
                          , const WOGMaterial &material, OGWorld* world
                          , bool dynamic
                          )
{
    qreal x1, x2, y1, y2, length, angle;
    OGPhysicsBody* line;
    OGPhysicsEngine* engine;

    engine = OGPhysicsEngine::GetInstance();


    x1 = anchor.x()*K*0.5;
    y1 = anchor.y()*K*0.5;

    //0.55 = (length + 10%)/2
    length =  qMax(world->scenesize().width()
                   , world->scenesize().height()
                   )*0.55*K;

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
        line->CreateFixture(0.0, material.friction*0.01, material.bounce);
    }
    else
    {
        line->CreateFixture(0.0, material.friction*0.01, material.bounce);
    }

    return line;
}

OGPhysicsBody* createRectangle(const QPointF & position, const QSizeF & size
                               , qreal rotation, const WOGMaterial & material
                               , bool dynamic
                          )
{
    OGPhysicsBody* rect;
    OGPhysicsEngine* engine;

    engine = OGPhysicsEngine::GetInstance();

    rect = new OGPhysicsBody(position.x()*K, position.y()*K , dynamic);

    engine->CreateBody(rect);
    rect->CreateShape(OGPhysicsBody::POLYGON);
    rect->shape->SetAsBox(size.width()*K*0.5, size.height()*K*0.5
                            , b2Vec2(0, 0)
                            , rotation);

    if (dynamic)
    {
        rect->CreateFixture(0.0, material.friction*0.01, material.bounce);
    }
    else
    {
        rect->CreateFixture(0.0, material.friction*0.01, material.bounce);
    }

    return rect;
}
