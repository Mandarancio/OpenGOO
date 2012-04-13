#include "balloongoo.h"
#include "goo.h"
#include "tools.h"

BalloonGoo::BalloonGoo(b2World *world, QPoint p, int radius, QObject *parent):
    DynamicGoo(world,p,radius,parent)
{

    color=Qt::yellow;

    maxJoints=1; //parameters
    type=BALOON;
}

void BalloonGoo::paint(QPainter &p){
    if (isSleeping()) emit checkForNeighbors(getPPosition());

    this->moveToTarget();

    p.setPen(Qt::darkGray);

    p.setBrush((isSleeping() ? Qt::darkGray :  Qt::yellow));
    if (!active){
        p.drawEllipse(toPoint(body->GetPosition()),getRadius(),getRadius());
    }
    else{
        p.drawEllipse(toPoint(body->GetPosition()),2*getRadius(),2*getRadius());
    }
}

bool BalloonGoo::createLink(Goo *goo){
    if (!active && nJoints()<maxJoints){
        active=true;
        body->SetGravityScale(-1);
        if (sleeping) sleeping=false;
        links.push_back(goo);
        if (following) stopFollow();
        return true;
    }
    else return false;
}

bool BalloonGoo::destroyLink(Goo *goo){
    if (isLinked(goo)){
        links.removeAt(links.indexOf(goo));
        emit this->destroyJoint(this,goo);
        active=false;
        body->SetGravityScale(1);
        if (!isDragging() && !hasJoint()) body->SetActive(false);
        return true;
    }
    else return false;
}

void BalloonGoo::drag(){
    onGround=false;
    stopFollow();
    if (!dragging){
        info.gScale=body->GetGravityScale();
        info.speed=body->GetLinearVelocity();
        info.aForce=body->GetAngularVelocity();
    }
    body->SetActive(false);
    if (hasJoint()) {
        for (int i=0;i<links.count();i++) destroyLink(links.at(i));
    }
    dragging=true;

}

bool BalloonGoo::isDragable(){
    if (!isSleeping()) return true;
    else return false;
}

void BalloonGoo::paintDebug(QPainter &p){
    p.setPen(Qt::darkGray);
    p.setBrush(Qt::transparent);

    p.drawEllipse(toPoint(body->GetPosition()),getRadius(),getRadius());
}
