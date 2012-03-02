#include "ground.h"
#include "tools.h"
#include <QDebug>


//Constructor
Ground::Ground(b2World *world, QPoint center, QList<QPoint> shape, QObject *parent):
    QObject(parent)
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

    //save the shape to a QPlygon for draw it.
    polShape=toPoly(shape,center);
}

//Return physical body of the ground
b2Body* Ground::getBody(){
    return body;
}

//Function to know if a point is conteined in the ground
bool Ground::contains(QPoint p){
    return polShape.containsPoint(p,Qt::OddEvenFill);
}

//Function to paint the ground
void Ground::paint(QPainter &p){
    p.setBrush(Qt::black);
    p.setPen(Qt::black);
    //Draw the shape
    p.drawPolygon(polShape);
}

//Function to create the shape starting from the points read from the level files
void Ground::makeShape(QList<QPoint> points){
    //b2EdgeShape is a shape made of segments
    b2EdgeShape* shape;
    //make a segment for all the points
    for (int i=0;i<points.length()-1;i++){
        shape=new b2EdgeShape();
        shape->Set(toVec(points[i]),toVec(points[i+1]));
        body->CreateFixture(shape,1.0);
    }
}
