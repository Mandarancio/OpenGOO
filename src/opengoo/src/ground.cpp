#include "ground.h"
#include "tools.h"
#include <QDebug>
#include <QImage>



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
    def.angularDamping=0.0;
    def.linearDamping=0.0;
    //create the body;
    body= world->CreateBody(&def);
    //create the shape
    makeShape(shape);
    //save the shape to a QPlygon for draw it.
    polShape=toPoly(shape,center);
}

//Function to know if a goo is conteined in the ground
bool Ground::contains(Goo*goo){
    if (polShape.intersected(goo->boundingRect()).count()) return true;
    return false;
}

bool Ground::contains(QPoint p, int r){
    float rx=r/sqrt(2.0);

    if (contains(p)) return true;
    if (contains(QPoint(p.x(),p.y()-r))) return true;
    if (contains(QPoint(p.x(),p.y()+r))) return true;
    if (contains(QPoint(p.x()-r,p.y()))) return true;
    if (contains(QPoint(p.x()+r,p.y()))) return true;
    if (contains(QPoint(p.x()+rx,p.y()+rx))) return true;
    if (contains(QPoint(p.x()-rx,p.y()+rx))) return true;
    if (contains(QPoint(p.x()-rx,p.y()-rx))) return true;
    if (contains(QPoint(p.x()+rx,p.y()-rx))) return true;
    return false;
}


//Function to paint the ground
void Ground::paint(QPainter &p){

    //Draw the shape
    p.setBrush(Qt::black);
    p.setPen(Qt::black);

    if (polShape.count()) p.drawPolygon(polShape);

}

//Function to create the shape starting from the points read from the level files
void Ground::makeShape(QList<QPoint> points){
    //b2EdgeShape is a shape made of segments
    b2EdgeShape* shape;
    //make a segment for all the points
    for (int i=0;i<points.length();i++){
        if (i==points.length()-1){
            shape=new b2EdgeShape();
            shape->Set(toVec(points[i]),toVec(points[0]));
            body->CreateFixture(shape,1.0);
        }
        else {
            shape=new b2EdgeShape();
            shape->Set(toVec(points[i]),toVec(points[i+1]));
            body->CreateFixture(shape,1.0);
        }
    }
}
