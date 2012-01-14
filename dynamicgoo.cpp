#include "dynamicgoo.h"
#include "tools.h"

DynamicGoo::DynamicGoo(b2World *world, QPoint p, int radius,  QObject *parent):
    Goo(radius,parent)
{
    b2BodyDef def;
    def.awake=true;
    def.type=b2_dynamicBody;
    def.position=toVec(p);
    body=world->CreateBody(&def);
    b2CircleShape shape;
    shape.m_p.SetZero();
    shape.m_radius=radius;
    b2FixtureDef fixDef;
    fixDef.restitution=0.3;
    fixDef.density=1.0;
    fixDef.friction=0.8;
    fixDef.shape=&shape;
    fixDef.userData=this;
    body->CreateFixture(&fixDef);
    body->SetLinearDamping(0.1);//Not sure about this parameter
    moovable=true;
    dragable=true;
    maxJoints=7;
    speed=50;
}

void DynamicGoo::catched(){
    speed*=2;
}

void DynamicGoo::lost(){
    speed/=2;
}

void DynamicGoo::moveToTarget(){
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
            if (abs(mePrev.x())>abs(targetPrev.x())+12 || abs(mePrev.y())>abs(targetPrev.y())+12){
                stopFollow();
                fallDown();
                return;
            }
        }
        b2Vec2 dP;
        if (target!=NULL /*&& target->getBody()!=NULL*/)
            dP=target->getVPosition()-this->getVPosition();
        else {
            stopFollow();
            fallDown();
            return;
        }
        if (dP.Length()<=4){
            emit this->nextTargetPlease(target);
            body->SetLinearVelocity(b2Vec2(0,0));
            return;
        }
        dP.x=dP.x*speed/dP.Length();
        dP.y=dP.y*speed/dP.Length();
        body->SetLinearVelocity(dP);
    }
}


void DynamicGoo::paint(QPainter &p){
    moveToTarget();
    p.setPen(Qt::black);
    p.setBrush(Qt::black);
    p.drawEllipse(toPoint(body->GetPosition()),getRadius(),getRadius());
}
