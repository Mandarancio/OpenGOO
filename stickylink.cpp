#include "stickylink.h"
#include "tools.h"

StickyLink::StickyLink(Goo *goo, b2Body *ground, QPoint contactPoint, b2World *world, int strength, QObject *parent) :
    QObject(parent)
{
    this->goo = goo;
    this->strength=strength; //Maximum force
    b2DistanceJointDef def;// Definition of the joint
    def.Initialize(goo->getBody(),ground,goo->getVPosition(),toVec(contactPoint)); //Initialization of the definition
    def.collideConnected=true;
    def.dampingRatio=1.0;
    def.frequencyHz=10;

   joint=(b2DistanceJoint*)world->CreateJoint(&def); //Creation of the phisical joint
}

void StickyLink::checkStatus(){
    if (!joint) return;
    if (joint->GetReactionForce(1.0/60).Length()>strength) emit destroySticky(); //If the force > of the maximum force barke the link!
}


b2Joint* StickyLink::getJoint(){
    return joint; //return the phisical join
}
void StickyLink::setJoint(b2Joint *j){
    joint=(b2DistanceJoint*)j;
}

Goo* StickyLink::getGoo(){
    return goo;
}


void StickyLink::paint(QPainter &p){
    p.setPen(Qt::red);
    p.drawLine(goo->getPPosition(),toPoint(joint->GetAnchorB()));
}
