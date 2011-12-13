#include "target.h"
#include "tools.h"

Target::Target(QPoint position, int height,b2World *world, QObject *parent) :
    QObject(parent)
{
    this->position=position;
    h=height;
    b2BodyDef def;
    def.type=b2_staticBody;
    def.position=toVec(position);
    b2PolygonShape shape;
    shape.SetAsBox(18,h/2,b2Vec2(0,-h/2),0);
    body=world->CreateBody(&def);
    body->CreateFixture(&shape,1.0);
    body->SetUserData(this);
}

void Target::paint(QPainter &p){
    p.setBrush(Qt::black);
    p.setPen(Qt::black);
    p.drawRect(position.x()-18,position.y(),18*2,-h);
    p.drawRect(position.x()-22,position.y()+5,22*2,-15);
}
