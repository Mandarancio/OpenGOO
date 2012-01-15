#include "goo.h"
#include "tools.h"

#include <QDebug>

Goo::Goo( int radius, QObject *parent) :
    QObject(parent)
{
    this->radius=radius;
    this->maxJoints=0;
    this->minJoints=0;

    guestN=0;

    moovable=false;
    falling=true;
    dragging=false;
    following=false;
    dragable=false;
    onGround=false;
    groundPoint=QPoint(0,0);

    info.aForce=0;
    info.gScale=1.0;
    info.speed.SetZero();


    target=NULL;
    prevTarget=NULL;
}

bool Goo::isOnGround(){
    //if (hasJoint() && isMoovable()) qWarning()<<"Has joint and is "<<onGround;
   // if (onGround && getPPosition()!=groundPoint) onGround=false;
    return onGround;
}

bool Goo::isDragable(){
    return dragable;
}

bool Goo::isMoovable(){
    return moovable;
}

bool Goo::isFalling(){
    return falling;
}

bool Goo::isDragging(){
    return dragging;
}

bool Goo::hasJoint(){
    if (links.length()>0) return true;
    else return false;
}

bool Goo::canHaveJoint(){
    if (hasJoint() && nJoints()<maxJoints) return true;
    else return false;
}

bool Goo::isLinked(Goo *goo){
    return links.contains(goo);
}

int Goo::nJoints(){
    return links.length();
}

int Goo::getRadius(){
    return radius;
}

int Goo::getMaxJoints(){
    return maxJoints;
}

int Goo::getGuestNumber(){
    return guestN;
}

bool Goo::newGuest(){
    if (guestN<5) {
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

void Goo::move(QPoint p){
    if (isMoovable())
        body->SetTransform(toVec(p),0);
}

void Goo::catched(){}
void Goo::lost(){}

void Goo::jumpTo(QPoint p){
    stopFollow();
    this->dragable=false;
    this->moovable=false;
    this->falling=true;
    body->SetGravityScale(0);
    b2Vec2 v=toVec(p)-body->GetPosition();
    v.x*=250/v.Length();
    v.y*=250/v.Length();
    body->SetAngularVelocity(0);
    body->SetLinearVelocity(v);
}

b2Body* Goo::getBody(){
    return body;
}

b2Vec2 Goo::getVPosition(){
    if (body==NULL && !body->IsActive()) return b2Vec2(0,0);
    return body->GetPosition();
}

QPoint Goo::getPPosition(){
    if (body==NULL || !body->IsActive()) return QPoint(0,0);
    return toPoint(body->GetPosition());
}

bool Goo::createLink(Goo *goo){
    if (!isLinked(goo) && nJoints()<maxJoints) {
        links.push_back(goo);
        if (following) stopFollow();
        return true;
    }
    else return false;
}

bool Goo::destroyLink(Goo *goo){
    if (isLinked(goo)){
        links.removeAt(links.indexOf(goo));
        emit this->loseLink(goo);

        return true;
    }
    else return false;
}

void Goo::contactGround(){
    onGround=true;
    groundPoint=this->getPPosition();
    if (falling) {
        falling=false;
        emit nextTargetPlease(NULL);
    }
}

void Goo::destroyThis(){
    for (int i=0;i<links.length();i++){
        emit this->loseLink(links[i]);
        links[i]->destroyLink(this);
        emit this->destroyJoint(this,links[i]);
    }
    emit this->destroyGoo();
}

void Goo::setTarget(Goo *goo){
        if (isFalling()) falling=false;
        if (prevTarget!=NULL) prevTarget->removeGuest();
        body->SetGravityScale(0);
        onGround=false;
        prevTarget=target;
        target=goo;
        connect(goo,SIGNAL(loseLink(Goo*)),this,SLOT(checkForConnection(Goo*)),Qt::QueuedConnection);
        following=true;
}

void Goo::paint(QPainter &p){

}


void Goo::drag(){
    onGround=false;
    stopFollow();
    if (!dragging){
        info.gScale=body->GetGravityScale();
        info.speed=body->GetLinearVelocity();
        info.aForce=body->GetAngularVelocity();
    }

    dragging=true;

    body->SetLinearVelocity(b2Vec2(0,0));
    body->SetGravityScale(0);
    body->SetAngularVelocity(0.0);

}

void Goo::drop(){
    stopFollow();
    body->SetLinearVelocity(info.speed);
    body->SetGravityScale(1.0);
    body->SetAngularVelocity(info.aForce);
    falling=true;
    dragging=false;
}

void Goo::drop(b2Vec2 speed){
    stopFollow();
    body->SetLinearVelocity(b2Vec2(0,0));

    body->SetGravityScale(1.0);
    body->SetAngularVelocity(info.aForce);
    body->ApplyLinearImpulse(speed,body->GetPosition());
    falling=true;
    dragging=false;
}

QList<Goo*> Goo::getLinks(){
    return links;
}

Goo* Goo::getPrevious(){
    return prevTarget;
}

void Goo::moveToTarget(){

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
    onGround=false;
    falling=true;
    body->SetLinearVelocity(b2Vec2(0,0));
    body->SetGravityScale(1.0);
    body->SetAngularVelocity(0);
}


