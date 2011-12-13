#include "ground.h"
#include "tools.h"
#include <QDebug>

Ground::Ground(b2World *world, QPoint center, QList<QPoint> shape, QObject *parent):
    QObject(parent)
{
    b2BodyDef def;
    def.type=b2_staticBody;
    def.position=toVec(center);
    body= world->CreateBody(&def);
    makeShape(shape);

    polShape=toPoly(shape,center);
}

void Ground::paint(QPainter &p){
    p.setBrush(Qt::black);
    p.setPen(Qt::black);
    p.drawPolygon(polShape);
}

void Ground::makeShape(QList<QPoint> points){
    b2EdgeShape* shape;
    for (int i=0;i<points.length()-1;i++){
        shape=new b2EdgeShape();
        shape->Set(toVec(points[i]),toVec(points[i+1]));
        body->CreateFixture(shape,1.0);
    }
}
