#include "ropejoint.h"

#include <qmath.h>
#include <QDebug>
#include <QPen>
#include "tools.h"

RopeJoint::RopeJoint(Goo *a, Goo *b, b2World *world, QObject *parent) :
    Joint(a,b,world,true,parent)
{
    this->world=world;
    initialize(world);
    type=ROPE;
}

RopeJoint::~RopeJoint(){
    for (int i=0;i<joints.length();i++){
        world->DestroyJoint(joints[i]);
    }
    joints.clear();
    for (int i=0;i<bodies.length();i++){
        world->DestroyBody(bodies[i]);
    }
    bodies.clear();


}


void RopeJoint::initialize(b2World *world){
    b2RopeJointDef jDef;
    jDef.bodyA=a->getBody();
    jDef.bodyB=b->getBody();
    jDef.localAnchorA=a->getVPosition();
    jDef.localAnchorB=b->getVPosition();
    jDef.maxLength=500;
    float x,y,x0,y0;
    x0=a->getVPosition().x;
    y0=a->getVPosition().y;
    float mx,my;


    mx=b->getVPosition().x-a->getVPosition().x;
    my=b->getVPosition().y-a->getVPosition().y;
    float d=qSqrt(mx*mx+my*my);
    mx/=d;
    my/=d;
    int n,k;
    n=30;
    k=d/n;

    b2PolygonShape shape;
    shape.SetAsBox(k*mx/2,k*my/2);

    b2FixtureDef fd;
    fd.shape = &shape;
    fd.density = 0.0;
    fd.friction = 0.2f;

    fd.filter.categoryBits = 0x0001;
    fd.filter.maskBits = 0xFFFF & ~0x0002;

    b2RevoluteJointDef jd;
    jd.collideConnected = false;

    float d2=500-d;
    int n2=d2/k;
    b2Body* prevBody = a->getBody();

    for (int i=0; i<n2/2;i++){
        x=x0+i*k;
        y=y0;
        b2BodyDef bd;
        bd.type = b2_dynamicBody;
        bd.position.Set(x,y);

        b2Body* body = world->CreateBody(&bd);
        body->CreateFixture(&fd);
        body->SetGravityScale(0);
        b2Vec2 anchor(x, y);
        jd.Initialize(prevBody, body, anchor);

        joints.push_back(world->CreateJoint(&jd));
        bodies.push_back(body);

        prevBody = body;
    }

    for (int i=0;i<n2/2;i++){
        x=x0+n2/2*k-i*k;
        y=y0;

        b2BodyDef bd;
        bd.type = b2_dynamicBody;
        bd.position.Set(x,y);

        b2Body* body = world->CreateBody(&bd);
        body->CreateFixture(&fd);
        body->SetGravityScale(0);
        b2Vec2 anchor(x, y);
        jd.Initialize(prevBody, body, anchor);

        joints.push_back(world->CreateJoint(&jd));
        bodies.push_back(body);

        prevBody = body;
    }

    for (int i=0;i<n-1;i++){
        x=x0+mx*(i*k);
        y=y0+my*(i*k);

        b2BodyDef bd;
        bd.type = b2_dynamicBody;
        bd.position.Set(x,y);

        b2Body* body = world->CreateBody(&bd);
        body->CreateFixture(&fd);
        body->SetGravityScale(0.2);
        b2Vec2 anchor(x, y);
        jd.Initialize(prevBody, body, anchor);

        joints.push_back(world->CreateJoint(&jd));
        bodies.push_back(body);

        prevBody = body;
    }


    jd.Initialize(prevBody, b->getBody(), b->getVPosition());

    joints.push_back(world->CreateJoint(&jd));
    joint=(b2RopeJoint*)world->CreateJoint(&jDef);
    b->move(QPoint(mx*500,my*500));

}

void RopeJoint::paint(QPainter &p){
    if (!a->isLinked(b) || !b->isLinked(a)) emit this->destroyJoint(this);
    QPen pen(Qt::black,3);
    p.setPen(pen);
    for (int i=0;i<bodies.length()-1;i++){
        p.drawLine(toPoint(bodies[i]->GetPosition()),toPoint(bodies[i+1]->GetPosition()));
    }
    p.drawLine(a->getPPosition(),b->getPPosition());
}



