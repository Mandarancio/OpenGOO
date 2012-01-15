#include "ground.h"
#include "tools.h"
#include <QDebug>

Ground::Ground(b2World *world, QPoint center, QList<QPoint> shape, QObject *parent):
    QObject(parent)
{
    //Body definition
    b2BodyDef def;
    def.type=b2_staticBody; //b2_staticBody ignore all forces and interact only with dynamic body.
    def.position=toVec(center); //center of the body
    body= world->CreateBody(&def); //create the body;
    makeShape(shape); //create the shape

    polShape=toPoly(shape,center); //save the shape
}

b2Body* Ground::getBody(){
    return body; //Return physical body
}

void Ground::paint(QPainter &p){
    p.setBrush(Qt::black);
    p.setPen(Qt::black);
    p.drawPolygon(polShape); //Dray the shape
}

void Ground::makeShape(QList<QPoint> points){
    b2EdgeShape* shape; //b2EdgeShape is a shape made of segments
    for (int i=0;i<points.length()-1;i++){ //make a segment for all the points
        shape=new b2EdgeShape();
        shape->Set(toVec(points[i]),toVec(points[i+1]));
        body->CreateFixture(shape,1.0);
    }
}
