#include "ropejoint.h"

#include <QPen>
#include "tools.h"

RopeJoint::RopeJoint(Goo *a, Goo *b, b2World *world, QObject *parent) :
    Joint(a,b,world,true,parent)
{
    this->world=world;
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
    jDef.maxLength=200;
    joint=(b2RopeJoint*)world->CreateJoint(&jDef);
    float x,y,x0,y0;
    x0=a->getVPosition().x;
    y0=a->getVPosition().y;
    float mx,my;
    mx=b->getVPosition().x-a->getVPosition().y;
    my=b->getVPosition().y-a->getVPosition().y;
    int n,k;
    n=50;
    k=jDef.maxLength/n;

    b2PolygonShape shape;
    shape.SetAsBox(k/2,k/4);

    b2FixtureDef fd;
    fd.shape = &shape;
    fd.density = 20.0f;
    fd.friction = 0.2f;
    fd.filter.categoryBits = 0x0001;
    fd.filter.maskBits = 0xFFFF & ~0x0002;

    b2RevoluteJointDef jd;
    jd.collideConnected = false;

    b2Body* prevBody = a->getBody();
    for (int i=0;i<n-1;i++){
        x=x0+mx*(i*k);
        y=y0+my*(i*k);

        b2BodyDef bd;
        bd.type = b2_dynamicBody;
        bd.position.Set(x,y);

        b2Body* body = world->CreateBody(&bd);
        body->CreateFixture(&fd);

        b2Vec2 anchor(x, y);
        jd.Initialize(prevBody, body, anchor);

        joints.push_back(world->CreateJoint(&jd));
        bodies.push_back(body);

        prevBody = body;
    }

    jd.Initialize(prevBody, b->getBody(), b->getVPosition());
    joints.push_back(world->CreateJoint(&jd));

}

void RopeJoint::paint(QPainter &p){
    QPen pen(Qt::black,3);
    p.setPen(pen);
    for (int i=0;i<bodies.length()-1;i++){
        p.drawLine(toPoint(bodies[i]->GetPosition()),toPoint(bodies[i+1]->GetPosition()));
    }

}


