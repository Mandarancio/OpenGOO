#include "balloongoo.h"
#include "goo.h"
#include "tools.h"

BalloonGoo::BalloonGoo(b2World *world, QPoint p, int radius, QObject *parent):
    DynamicGoo(world,p,radius,parent)
{

    color=Qt::yellow;

    maxJoints=1; //parameters
    maxGuest=0;
    guestN=0;
    rx=0;
    ry=0;
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
        body->ApplyForceToCenter(b2Vec2(0,-(5*body->GetWorld()->GetGravity().y)));
        p.drawEllipse(QPoint(body->GetPosition().x,body->GetPosition().y-(getRadius()+ry)),2*getRadius()+rx,2*getRadius()+ry);
    }
}

bool BalloonGoo::createLink(Goo *goo){
    if (!active && nJoints()<maxJoints){
        active=true;
        body->SetGravityScale(0);
        if (sleeping) sleeping=false;
        links.push_back(goo);
        if (following) stopFollow();
        rx=rand()%6-4;
        ry=rand()%6-3;
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
         rx=0;
         ry=0;
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
