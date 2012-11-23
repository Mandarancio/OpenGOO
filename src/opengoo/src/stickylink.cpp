#include "stickylink.h"
#include "tools.h"

#include "publicclass.h"

#include <QDebug>
StickyLink::StickyLink(Goo *goo, b2Body *ground, QPoint contactPoint, b2World *world, double strength, QObject *parent) :
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
   // logWarn()<<joint->GetReactionForce(1.0/60).Length();
    if (goo==NULL || joint==NULL) return;
    if (goo->isDragging() || !goo->hasJoint()) emit destroySticky();
    else if (joint->GetReactionForce(realStep/4.0).Length()>strength){
        //logWarn()<<this->strength<<joint->GetReactionForce(1.0/60.0).Length();
        emit destroySticky(); //If the force > of the maximum force barke the link!
    }
}


void StickyLink::paint(QPainter &p){
    p.setPen(Qt::red);
    if (!joint || !goo) return;
    p.drawLine(goo->getPPosition(),toPoint(joint->GetAnchorB()));
    QPoint m=(goo->getPPosition()+toPoint(joint->GetAnchorB()))/2;
    int x,y;
    if (goo->getPPosition().x()<joint->GetAnchorB().x){
        x=joint->GetAnchorB().x-goo->getPPosition().x();
        y=joint->GetAnchorB().y-goo->getPPosition().y();
    }
    else {
        x=goo->getPPosition().x()-joint->GetAnchorB().x;
        y=goo->getPPosition().y()-joint->GetAnchorB().y;
    }
    float r=atan2(static_cast<float>(y),static_cast<float>(x));
    p.save();
    p.translate(m);
    p.rotate(r);
    p.drawText(0,0,QString::number((joint->GetReactionForce(1.0/60).y<0? -1:+1)*joint->GetReactionForce(1.0/60).Length()));
    p.restore();
}
