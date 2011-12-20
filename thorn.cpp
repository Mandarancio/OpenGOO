#include "thorn.h"
#include "tools.h"
#define PI 3.141628
Thorn::Thorn(QPoint p, int h, float a, b2World *world, QObject *parent):
    QObject(parent)
{
    b2BodyDef def;
    def.type=b2_staticBody;
    def.position=toVec(p);
    def.angle=a;
    b2PolygonShape shape;
    b2Vec2 vecs[3];
    vecs[0].Set(-5,0);
    vecs[2].Set(0,h);
    vecs[1].Set(5,0);
    shape.Set(vecs,3);
    body=world->CreateBody(&def);
    body->CreateFixture(&shape,0.0);
    body->SetUserData(this);
    polygon.setPoint(0,QPoint(p.x()-5,p.y()));
    polygon.setPoint(1,QPoint(p.x(),p.y()-h));
    polygon.setPoint(2,QPoint(p.x()+5,p.y()));
    angle=a/PI*180;
}

void Thorn::paint(QPainter &p){
    p.setBrush(Qt::black);
    p.setPen(Qt::black);
    p.save();
    p.rotate(angle);
    p.drawPolygon(polygon);
    p.restore();
}
