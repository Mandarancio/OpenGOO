#include "goo.h"
#include "tools.h"

#include <QDebug>

Goo::Goo( int radius, QObject *parent) :
    QObject(parent)
{
    this->radius=radius;
    this->maxJoints=0;

    guestN=0;

    moovable=false;
    falling=true;
    dragging=false;
    following=false;
    dragable=false;

    target=NULL;
    prevTarget=NULL;
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
    if (links.length()) return true;
    else return false;
}

bool Goo::canHaveJoint(){
    if (hasJoint() && nJoints()<maxJoints) return true;
    else false;
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

b2Body* Goo::getBody(){
    return body;
}

b2Vec2 Goo::getVPosition(){
    return body->GetPosition();
}

QPoint Goo::getPPosition(){
    return toPoint(body->GetPosition());
}

bool Goo::createLink(Goo *goo){
    if (!isLinked(goo) && nJoints()<maxJoints) {
        links.push_back(goo);
        return true;
    }
    else return false;
}

bool Goo::destroyLink(Goo *goo){
    if (isLinked(goo)){
        links.removeOne(goo);
        emit this->loseLink(goo);
        return true;
    }
    else return false;
}

void Goo::contactGround(){
    if (falling) {
        falling=false;
        emit nextTargetPlease(NULL);
    }
}

void Goo::setTarget(Goo *goo){
        if (prevTarget!=NULL) prevTarget->removeGuest();
        prevTarget=target;
        target=goo;
        connect(goo,SIGNAL(loseLink(Goo*)),this,SLOT(checkForConnection(Goo*)));
        following=true;
}

void Goo::paint(QPainter &p){
        moveToTarget();
    p.setPen(Qt::black);
    p.setBrush(Qt::black);
    p.drawEllipse(toPoint(body->GetPosition()),radius,radius);
}


void Goo::drag(){
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
    body->SetLinearVelocity(speed);
    body->SetGravityScale(1.0);
    body->SetAngularVelocity(info.aForce);
    falling=true;
    dragging=false;
}

QList<Goo*> Goo::getLinks(){
    return links;
}

void Goo::moveToTarget(){
    if (dragging) return;
    if (hasJoint()) return;
    if (!hasJoint() &&!falling&&!following) emit this->nextTargetPlease(NULL);
    if (following && !falling){
        if (prevTarget){
//            if (!target->isLinked(prevTarget)){
//                drop();
//                return;
//            }
            QPoint mePrev=getPPosition()-prevTarget->getPPosition();
            QPoint targetPrev=target->getPPosition()-prevTarget->getPPosition();
            if (abs(mePrev.x())>abs(targetPrev.x())+10 || abs(mePrev.y())>abs(targetPrev.y())+10){
                drop();
                return;
            }
        }
        b2Vec2 dP;
        if (target)
            dP=target->getVPosition()-this->getVPosition();
        if (dP.Length()<=1){
            emit this->nextTargetPlease(target);
            body->SetLinearVelocity(b2Vec2(0,0));
            return;
        }
        dP.x=dP.x*50/dP.Length();
        dP.y=dP.y*50/dP.Length();
        body->SetLinearVelocity(dP);
    }
}

void Goo::stopFollow(){
    if (following){
        disconnect(target,SIGNAL(loseLink(Goo*)),this,SLOT(checkForConnection(Goo*)));
        target->removeGuest();
        following=false;
        target=NULL;
        prevTarget=NULL;
    }
}

void Goo::checkForConnection(Goo *goo){
    if (following && prevTarget==goo){
        drop();
    }
}
