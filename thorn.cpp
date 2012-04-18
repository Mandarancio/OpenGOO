#include "thorn.h"
#include "tools.h"
#include <qmath.h>
#define PI 3.141628
Thorn::Thorn(QPoint center,QList<QPoint>shape, b2World *world, QObject *parent):
    Object(parent)
{
    //Body definition
    b2BodyDef def;
    //b2_staticBody ignore all forces and interact only with dynamic body.
    def.type=b2_staticBody;
    //center of the body
    def.position=toVec(center);
    //create the body;
    body= world->CreateBody(&def);
    //create the shape
    makeShape(shape);
    polygon=toPoly(shape,center);

}

void Thorn::makeShape(QList<QPoint> points){
    //b2EdgeShape is a shape made of segments
    b2EdgeShape* shape;
    //make a segment for all the points
    for (int i=0;i<points.length()-1;i++){
        shape=new b2EdgeShape();
        shape->Set(toVec(points[i]),toVec(points[i+1]));
        body->CreateFixture(shape,1.0);
    }
}

void Thorn::paint(QPainter &p){
    p.setBrush(Qt::black);
    p.setPen(Qt::black);
    p.drawPolygon(polygon);
}
