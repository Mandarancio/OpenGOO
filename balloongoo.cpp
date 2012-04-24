#include "balloongoo.h"
#include "goo.h"
#include "tools.h"

#include <QPen>

BalloonGoo::BalloonGoo(b2World *world, QPoint p, int radius, QObject *parent):
    DynamicGoo(world,p,radius,parent)
{

    color=Qt::yellow;
    distanceToJoint=200;

    maxJoints=1; //parameters
    maxGuest=0;
    guestN=0;
    rx=0;
    ry=0;
    stdRadius=radius;
    type=BALOON;
}

void BalloonGoo::paint(QPainter &p){
    if (isSleeping()) emit checkForNeighbors(getPPosition());

    this->moveToTarget();

    p.setPen(Qt::black);

    p.setBrush(Qt::black);
    //not active status
    if (!active){
        p.drawEllipse(toPoint(body->GetPosition()),getRadius(),getRadius());
        if (selected || dragging ){
            p.setPen(QPen(Qt::yellow,3,(dragging==true ? Qt::SolidLine : Qt::DashLine)));
            p.setBrush(Qt::transparent);
            p.drawEllipse(toPoint(body->GetPosition()), getRadius()+10,getRadius()+10);
        }
    }
    //active status
    else{
        //apply the force to fly)
        body->ApplyForceToCenter(b2Vec2(0,-(10*body->GetWorld()->GetGravity().y)));

        p.setBrush(Qt::black);
        p.setPen(Qt::black);

        p.drawEllipse(QPoint(body->GetPosition().x,body->GetPosition().y-(getRadius()/2+ry)),getRadius()+rx,getRadius()+ry);
        p.save();
        p.translate(body->GetPosition().x,body->GetPosition().y-(getRadius()/2+ry));
        QPolygon pol(3);
        pol<<QPoint(radius+rx-12,radius-qAbs(rx)-qAbs(ry*2/3))<<QPoint(0,radius+ry+10)<<QPoint(-radius-rx+11,radius-qAbs(rx)-qAbs(ry*2/3));
        p.drawPolygon(pol);
        p.restore();
        if(selected){
            p.setPen(QPen(Qt::yellow,3,Qt::DotLine));
            p.setBrush(Qt::transparent);
            int r=(rx>ry? getRadius()+rx : getRadius()+ry);
            p.drawEllipse(QPoint(body->GetPosition().x,body->GetPosition().y-(getRadius()/2+ry)),r+10,r+10);

        }
    }
}

bool BalloonGoo::createLink(Goo *goo){
    if (!active && nJoints()<maxJoints){
        //create a joint and make it fly!!
        active=true;
        body->SetGravityScale(0);
        if (sleeping) sleeping=false;
        links.push_back(goo);
        if (following) stopFollow();
        rx=-2;
        ry=3;
        radius=30;
        return true;
    }
    else return false;
}

bool BalloonGoo::destroyLink(Goo *goo){
    if (isLinked(goo)){
        //return at normality!
        links.removeAt(links.indexOf(goo));
        emit this->destroyJoint(this,goo);
        active=false;
        body->SetGravityScale(1);
        rx=0;
        ry=0;
        radius=stdRadius;
        if (!isDragging() && !hasJoint()) body->SetActive(false);
        return true;

    }
    else return false;
}

void BalloonGoo::drag(){
    //If dragging stop to fallow and to fly!
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
    //Always draggable
    if (!isSleeping()) return true;
    else return false;
}

void BalloonGoo::paintDebug(QPainter &p){
    p.isActive();
}
