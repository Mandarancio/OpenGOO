#include "stickyjoint.h"
#include "tools.h"



StickyJoint::StickyJoint(Goo *goo, Ground *ground, QPoint point, QObject *parent) :
    QObject(parent)
{
    //Set the properties
    this->goo=goo;
    this->ground=ground;
    this->point=point;
    b2DistanceJointDef def; //The phisical joint
    def.Initialize(goo->getBody(),ground->getBody(),goo->getVPosition(),toVec(point));
}

void StickyJoint::checkStatus(){}

void StickyJoint::paint(QPainter &p){
    p.setPen(Qt::red);
    p.drawLine(point,goo->getPPosition()); //Draw a line to show graphicaly the joint
}
