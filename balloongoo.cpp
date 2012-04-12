#include "balloongoo.h"
#include "tools.h"

BalloonGoo::BalloonGoo(b2World *world, QPoint p, int radius, QObject *parent):
    Goo(radius,parent)
{
    b2BodyDef def;
    def.type=b2_dynamicBody;
    def.position=toVec(p);
    body=world->CreateBody(&def);
    b2CircleShape shape;
    shape.m_p.SetZero();
    shape.m_radius=radius;
    b2FixtureDef fDef;
    fDef.userData=this;
    fDef.density=0.1;
    fDef.shape=&shape;
    body->CreateFixture(&fDef);
    maxJoints=1;
    onGround=false;
    groundPoint=p;
}

void BalloonGoo::paint(QPainter &p){
    p.setPen(Qt::darkGray);
    p.setBrush(Qt::yellow);
    p.drawEllipse(toPoint(body->GetPosition()),getRadius(),getRadius());
}

void BalloonGoo::paintDebug(QPainter &p){
    p.setPen(Qt::darkGray);
    p.setBrush(Qt::transparent);

    p.drawEllipse(toPoint(body->GetPosition()),getRadius(),getRadius());

}

void BalloonGoo::contactGround(){
 //TODO
}
void BalloonGoo::contactGround(QPoint p){
    if (p.isNull()) return;
 //TODO
}
