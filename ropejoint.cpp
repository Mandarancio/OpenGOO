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
    jDef.localAnchorA.SetZero();
    jDef.localAnchorB.Set(0,20);
    jDef.collideConnected=true;
    jDef.maxLength=250;
    joint=(b2RopeJoint*)world->CreateJoint(&jDef);

}

void RopeJoint::paint(QPainter &p){
    if (!a->isLinked(b) || !b->isLinked(a)) emit this->destroyJoint(this);

    p.setPen(QPen(Qt::black,2));
    for (int i=0;i<bodies.length()-1;i++){
        p.drawLine(toPoint(bodies[i]->GetPosition()),toPoint(bodies[i+1]->GetPosition()));
    }
    p.drawLine(toPoint(joint->GetAnchorA()),toPoint(joint->GetAnchorB())-QPoint(0,-5));
    p.setPen(QPen(Qt::black,2));

    p.drawLine(toPoint(joint->GetAnchorA())+QPoint(2,0),toPoint(joint->GetAnchorB())+QPoint(0,-2));
    p.drawLine(toPoint(joint->GetAnchorA())-QPoint(2,0),toPoint(joint->GetAnchorB())+QPoint(0,-2));

}



