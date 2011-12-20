#include "thorn.h"
#include "tools.h"
#include <qmath.h>
#define PI 3.141628
Thorn::Thorn(QPoint p, int h, b2World *world, QObject *parent):
    Object(parent)
{
    b2BodyDef def;
    def.type=b2_staticBody;
    def.position=toVec(p);
    b2PolygonShape shape;
    b2Vec2 vecs[3];
    vecs[2].Set(-7,h);
    vecs[0].Set(0,0);
    vecs[1].Set(7,h);
    shape.Set(vecs,3);
    body=world->CreateBody(&def);
    body->CreateFixture(&shape,0.0);
    body->SetUserData(this);
    polygon=QPolygon(3);
    polygon.setPoint(0,QPoint(p.x()-7,p.y()+h));
    polygon.setPoint(1,QPoint(p.x(),p.y()));
    polygon.setPoint(2,QPoint(p.x()+7,p.y()+h));
}

void Thorn::paint(QPainter &p){
    p.setBrush(Qt::black);
    p.setPen(Qt::black);
    p.drawPolygon(polygon);
}
