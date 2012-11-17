#include "balloongoo.h"
#include "goo.h"
#include "tools.h"

#include <QPen>
#include <qmath.h>

#include <QDebug>

BalloonGoo::BalloonGoo(b2World *world, QPoint p, int radius, QObject *parent):
    DynamicGoo(world,p,radius,parent)
{

    color=Qt::yellow;
    secondaryColor=Qt::yellow;
    distanceToJoint=200;

    maxJoints=1; //parameters
    maxGuest=0;
    guestN=0;
    rx=0;
    ry=0;
    stdRadius=radius;
    stickable=false;

    body->SetFixedRotation(true);

    type=BALOON;
    active=false;
}

void BalloonGoo::paint(QPainter &p){


    //not active status
    if (!active){
        p.setBrush(color);
        p.setPen(color);
        p.drawEllipse(toPoint(body->GetPosition()),getRadius(),getRadius());
        if (selected || dragging ){
            p.setPen(QPen(Qt::yellow,3,(dragging==true ? Qt::SolidLine : Qt::DashLine)));
            p.setBrush(Qt::transparent);
            p.drawEllipse(toPoint(body->GetPosition()), getRadius()+10,getRadius()+10);
        }
    }
    //active status
    else{
        p.setPen(color);

        p.setBrush(color);



        p.drawEllipse(QPoint(body->GetPosition().x*10,body->GetPosition().y*10-(getRadius()/2+ry)),
                      getRadius() + rx, getRadius() + ry );
        p.save();
        p.translate(body->GetPosition().x*10,body->GetPosition().y*10-(getRadius()/2+ry));
        QPolygon pol(3);
        pol<<QPoint(radius+rx-12,radius-qAbs(rx)-qAbs(ry*2/3))<<QPoint(0,radius+ry+10)<<QPoint(-radius-rx+11,radius-qAbs(rx)-qAbs(ry*2/3));
        p.drawPolygon(pol);
        p.restore();
        if(selected){
            p.setPen(QPen(Qt::yellow,3,Qt::DotLine));
            p.setBrush(Qt::transparent);
            int r=(rx>ry? getRadius()+rx : getRadius()+ry);
            p.drawEllipse(QPoint(body->GetPosition().x*10,body->GetPosition().y*10-(getRadius()/2+ry)),r+10,r+10);

        }
    }
}

bool BalloonGoo::createLink(Goo *goo){
    if (!active && nJoints()<maxJoints){
        //create a joint and make it fly!!
        active=true;
        if (sleeping) sleeping=false;
        links.push_back(goo);
        body->SetGravityScale(0.0);
        if (following) stopFollow();
        rx=-2;
        ry=3;
        radius=30;

        b2MassData mass;
        mass.center.SetZero();
        mass.I=0.1;
        mass.mass=0.5;
        body->SetMassData(&mass);
        body->SetLinearDamping(0.2);
        body->SetAngularDamping(0.2);


        force=-b2Vec2(0,(10*body->GetWorld()->GetGravity().y));

        return true;
    }
    else return false;
}

bool BalloonGoo::destroyLink(Goo *goo){
    if (isLinked(goo)){
        //return at normality!
        links.removeAt(links.indexOf(goo));
        emit this->destroyJoint(this,goo);
        goo->destroyLink(this);
        active=false;
        body->SetGravityScale(1);
        rx=0;
        ry=0;
        radius=stdRadius;

        b2MassData mass;
        mass.center.SetZero();
        mass.I=0.1;
        mass.mass=0.5;
        body->SetMassData(&mass);

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
        for (int i=0;i<links.count();i++) {
            destroyLink(links.at(i));
        }
    }


    dragging=true;

}

void BalloonGoo::paintDebug(QPainter &p){
    float fx,fy;
    float f;
    fx=body->GetLinearVelocity().x;
    fy=body->GetLinearVelocity().y;
    f=qSqrt(fx*fx+fy*fy);
    if (f<=0.0000001){
        return;
    }
    fx/=100;
    fy/=100;
    p.save();
    p.setPen(Qt::white);
    p.translate(this->getPPosition());
    p.drawLine(0,0,fx*40,fy*40);
    p.rotate(90+qAtan2(-fx,fy)*180.0/3.141628);
    p.translate(f*4/10,0);
    p.rotate(135);
    p.drawLine(0,0,8,0);
    p.rotate(90);
    p.drawLine(0,0,8,0);

    p.restore();
}

void BalloonGoo::update(){
    if (isSleeping()) emit checkForNeighbors(getPPosition());
    this->moveToTarget();
    if (active){
        //apply the force to fly
        body->ApplyForceToCenter(force);
        body->SetAngularVelocity(0.0);
        body->SetTransform(body->GetPosition(),0.0);
    }
}
