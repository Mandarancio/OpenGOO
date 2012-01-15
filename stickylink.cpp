#include "stickylink.h"
#include "tools.h"

StickyLink::StickyLink(Goo *goo, Ground *ground, QPoint contactPoint, b2World *world, int strength, QObject *parent) :
    QObject(parent)
{
    this->strength=strength; //Maximum force
    b2DistanceJointDef def;// Definition of the joint
    def.Initialize(goo->getBody(),ground->getBody(),goo->getVPosition(),toVec(contactPoint)); //Initialization of the definition
    joint=(b2DistanceJoint*)world->CreateJoint(&def); //Creation of the phisical joint
}

void StickyLink::checkStatus(){
    if (joint->GetReactionForce(1.0/60).Length()>strength) emit destroySticky(); //If the force > of the maximum force barke the link!
}

b2Joint* StickyLink::getJoint(){
    return joint; //return the phisical join
}
