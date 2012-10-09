#include "fixedgoo.h"
#include "tools.h"

FixedGoo::FixedGoo(b2World *world, QPoint p, int radius, QObject *parent):
    Goo(radius,parent)
{
    b2BodyDef def;
    def.type=b2_staticBody;
    def.position=toVec(p);
    body=world->CreateBody(&def);
    b2CircleShape shape;
    shape.m_p.SetZero();
    shape.m_radius=radius;
    b2FixtureDef fDef;
    fDef.userData=this;
    fDef.density=1.0;
    fDef.shape=&shape;
    body->CreateFixture(&fDef);
    maxJoints=5;
    onGround=true;
    groundPoint=p;
    type=FIXED_GOO;
}

void FixedGoo::paint(QPainter &p){
    p.setPen(Qt::black);
    p.setBrush(Qt::black);
    p.drawEllipse(toPoint(body->GetPosition()),getRadius(),getRadius());
}

void FixedGoo::paintDebug(QPainter &p){
    p.setPen(Qt::darkGray);
    p.setBrush(Qt::transparent);

    p.drawEllipse(toPoint(body->GetPosition()),getRadius(),getRadius());

}


void FixedGoo::update(){
    //DO SOMETHING
}

void FixedGoo::contactGround(){
 //TODO
}
void FixedGoo::contactGround(QPoint p){
    if (p.isNull()) return;
 //TODO
}
