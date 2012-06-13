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
    body->SetUserData(this);
    polygon=toPoly(shape,center);


}

void Thorn::makeShape(QList<QPoint> points){
    //b2EdgeShape is a shape made of segments
    b2PolygonShape* shape;
    b2Fixture* fix;
    float angle,x,y,w,h,dx,dy;
    h=0.1;
    //make a segment for all the points
    for (int i=0;i<points.length();i++){
        if (i==points.length()-1){
            shape=new b2PolygonShape();
            dx=(points[i].x()-points[0].x());
            dy=(points[i].y()-points[0].y());
            w=qSqrt(dx*dx+dy*dy)/20.0;
            x=(points[i].x()+points[0].x())/20;
            y=(points[i].y()+points[0].y())/20;
            angle=3.141628/2-qAtan2(dx,dy);
            shape->SetAsBox(w,h,b2Vec2(x,y),angle);

            //shape->Set(toVec(points[i]),toVec(points[i+1]));
            fix= body->CreateFixture(shape,1.0);
        }
        else {
            shape=new b2PolygonShape();
            dx=(points[i].x()-points[i+1].x());
            dy=(points[i].y()-points[i+1].y());
            w=qSqrt(dx*dx+dy*dy)/20.0;
            x=(points[i].x()+points[i+1].x())/20;
            y=(points[i].y()+points[i+1].y())/20;
            angle=3.141628/2-qAtan2(dx,dy);
            shape->SetAsBox(w,h,b2Vec2(x,y),angle);

            //shape->Set(toVec(points[i]),toVec(points[i+1]));
            fix= body->CreateFixture(shape,1.0);
        }
    }

}

void Thorn::paint(QPainter &p){
    p.setBrush(Qt::black);
    p.setPen(Qt::transparent);
    p.drawPolygon(polygon);
}
