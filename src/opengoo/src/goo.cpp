#include "goo.h"
#include "tools.h"

#include <QDebug>

#include "publicclass.h"

Goo::Goo( int radius, QObject *parent) :
    QObject(parent)
{
    //Initialize variable
    this->radius=radius;
    this->maxJoints=0;
    this->minJoints=0;

    guestN=0;
    links=QList<Goo*>();

    counted=false;
    selected=false;
    moovable=false;
    falling=true;
    dragging=false;
    following=false;
    dragable=false;
    onGround=false;
    sleeping=true;
    groundPoint=QPoint(0,0);

    info.aForce=0;
    info.gScale=1.0;
    info.speed.SetZero();

    maxGuest=100;
    distanceToJoint=150;

    stickness=0.01;

    target=NULL;
    prevTarget=NULL;
    type=NONE;

    ALbyte name[100]="resources/sounds/scream.wav";
    scream=sSystem->createSource(name);
}

//Check if is on ground
bool Goo::isOnGround(){
    if (getPPosition().x()-groundPoint.x()>5 || getPPosition().y()-groundPoint.y()>5) onGround=false;
    else onGround=true;
    return onGround;
}

int Goo::getNumberOf(GooType type){
    if (counted) return 0;
    int value=0;
    counted=true;
    if (this->type==type){
        value++;
    }
    for (int i=0;i<links.length();i++){
        value += links[i]->getNumberOf(type);
    }
    return value;
}

void Goo::countingEnd(){
    if (!counted) return;
    counted=false;
    for (int i=0;i<links.length();i++){
        links[i]->countingEnd();
    }
}

bool Goo::newGuest(){
    if (guestN<=maxGuest) {
        guestN++;
        return true;
    }
    else return false;
}

bool Goo::removeGuest(){
    if (guestN>0) {
        guestN--;
        return true;
    }
    else return false;
}

bool Goo::canHaveGuest(){
    if (maxGuest && guestN<maxGuest)
        return true;

    else return false;
}

void Goo::move(QPoint p){
    if (isMoovable())
        body->SetTransform(toVec(p),0);
}

void Goo::jumpTo(QPoint p){
    stopFollow();
    if (!falling){
        ALbyte name[100]="resources/sounds/captured.wav";
        QPair<unsigned int,unsigned int> source =sSystem->createSource(name);
        sSystem->setPitch(source.first,float(radius)/20.0);
        sSystem->setVolume(source.first,radius/24.0);
        sSystem->setPosition(source.first,getPPosition());
        sSystem->playSource(source.first);
        sSystem->addSource(source);
    }
    this->dragable=false;
    this->moovable=false;
    this->falling=true;
    body->SetGravityScale(0);
    b2Vec2 v=toVec(p)-body->GetPosition();
    v.x*=25/v.Length();
    v.y*=25/v.Length();
    body->SetAngularVelocity(0);
    body->SetLinearVelocity(v);

}


b2Vec2 Goo::getVPosition(){
    if (body==NULL) return b2Vec2(0,0);
    return body->GetPosition();
}
b2Vec2 Goo::getVPositionScaled(){
    if (body==NULL) return b2Vec2(0,0);
    return 10.0*body->GetPosition();
}

QPoint Goo::getPPosition(){
    if (body==NULL) return QPoint(0,0);
    return toPoint(body->GetPosition());
}

bool Goo::createLink(Goo *goo){
    if (!isLinked(goo) && nJoints()<maxJoints) {
        if (sleeping) sleeping=false;
        links.push_back(goo);
        if (following) stopFollow();
        body->SetAngularVelocity(0.0);
        body->SetFixedRotation(true);

        return true;
    }
    else return false;
}

bool Goo::destroyLink(Goo *goo){
    if (isLinked(goo)){
        links.removeAt(links.indexOf(goo));
        emit this->loseLink(goo);
//        if (dragging){
//            qWarning("removed link!");
//            b2Joint*j=body->GetWorld()->GetJointList()->GetNext();
//            for (int i=1;i<body->GetWorld()->GetJointCount();i++){
//                if (j==NULL) break;
//                if (j->GetBodyA()==body || j->GetBodyB()==body) {
//                    qWarning()<<"ERROR";
//                    b2Joint *n=j->GetNext();
//                    body->GetWorld()->DestroyJoint(j);
//                    j=n;
//                }
//                else
//                    j=j->GetNext();
//            }
//        }
        body->SetFixedRotation(false);
        if (isDragging() && !hasJoint()) body->SetActive(false);
        return true;

    }
    else return false;
}



void Goo::destroyThis(){
    for (int i=0;i<links.length();i++){
        emit this->loseLink(links[i]);
        links[i]->destroyLink(this);
        emit this->destroyJoint(this,links[i]);
    }
    emit this->destroyGoo();
}

QRect Goo::boundingRect(){
    QRect rect(getPPosition()-QPoint(radius,radius),QSize(radius*2,radius*2));
    return rect;
}

void Goo::setTarget(Goo *goo){
        if (isFalling()) falling=false;
        if (prevTarget!=NULL) prevTarget->removeGuest();
        body->SetGravityScale(0);
        //onGround=false;
        if (target!=NULL) prevTarget=target;
        target=goo;
        connect(goo,SIGNAL(loseLink(Goo*)),this,SLOT(checkForConnection(Goo*)),Qt::QueuedConnection);
        following=true;
}



void Goo::drag(){
    onGround=false;
    stopFollow();
    if (!dragging){
        info.gScale=body->GetGravityScale();
        info.speed=body->GetLinearVelocity();
        info.aForce=body->GetAngularVelocity();
    }
    if (!hasJoint())  {
        if (!dragging){
            ALbyte name[100]="resources/sounds/drag.wav";
            QPair<unsigned int,unsigned int> source =sSystem->createSource(name);
            sSystem->setPitch(source.first,20.0/float(radius));
            sSystem->setVolume(source.first,radius/24.0);
            sSystem->setPosition(source.first,getPPosition());
            sSystem->playSource(source.first);
            sSystem->addSource(source);
        }
        body->SetActive(false);
    }
    body->SetLinearVelocity(b2Vec2(0,0));
    body->SetAngularVelocity(0);
    dragging=true;



}

void Goo::drop(){
    stopFollow();
    if (!hasJoint()){
        body->SetLinearVelocity(info.speed);

        body->SetAngularVelocity(info.aForce);

    }
    body->SetGravityScale(1.0);
    body->SetActive(true);

    falling=true;
    dragging=false;
}

void Goo::drop(b2Vec2 speed){
    body->SetActive(true);
    body->SetGravityScale(1.0);
    if (!hasJoint())
        body->ApplyLinearImpulse(speed,b2Vec2(0,0));
    falling=true;
    dragging=false;
}

void Goo::stopFollow(){
    if (following){
        disconnect(this,SLOT(checkForConnection(Goo*)));
        if (target==NULL) return;
        //disconnect(target,SIGNAL(loseLink(Goo*)),this,SLOT(checkForConnection(Goo*)));
        target->removeGuest();
        following=false;
        target=NULL;
        prevTarget=NULL;
    }
}

void Goo::checkForConnection(Goo *goo){
    if (following && prevTarget==goo){
        prevTarget=NULL;
        target=NULL;
        stopFollow();
        fallDown();
    }
}

void Goo::fallDown(){
    if (!falling){
//        if (sSystem->sourceStatus(scream.first)) sSystem->stopSource(scream.first);
//        sSystem->setPitch(scream.first,float(radius)/20.0);
//        sSystem->setVolume(scream.first,radius/24.0);
//        sSystem->setPosition(scream.first,getPPosition());
//        sSystem->playSource(scream.first);
    }
    onGround=false;
    falling=true;
    stopFollow();
    following=false;
    prevTarget=NULL;
    target=NULL;
    body->SetGravityScale(1.0);
    body->SetAngularVelocity(0);
}
