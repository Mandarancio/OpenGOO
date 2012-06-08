#include "ground.h"
#include "tools.h"
#include <QDebug>
#include <QImage>
#include "graphicseffect.h"



QImage blurred(const QImage& image, const QRect& rect, int radius, bool alphaOnly = false)
{
    int tab[] = { 14, 10, 8, 6, 5, 5, 4, 3, 3, 3, 3, 2, 2, 2, 2, 2, 2 };
    int alpha = (radius < 1)  ? 16 : (radius > 17) ? 1 : tab[radius-1];

    QImage result = image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
    int r1 = rect.top();
    int r2 = rect.bottom();
    int c1 = rect.left();
    int c2 = rect.right();

    int bpl = result.bytesPerLine();
    int rgba[4];
    unsigned char* p;

    int i1 = 0;
    int i2 = 3;

    if (alphaOnly)
        i1 = i2 = (QSysInfo::ByteOrder == QSysInfo::BigEndian ? 0 : 3);

    for (int col = c1; col <= c2; col++) {
        p = result.scanLine(r1) + col * 4;
        for (int i = i1; i <= i2; i++)
            rgba[i] = p[i] << 4;

        p += bpl;
        for (int j = r1; j < r2; j++, p += bpl)
            for (int i = i1; i <= i2; i++)
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
    }

    for (int row = r1; row <= r2; row++) {
        p = result.scanLine(row) + c1 * 4;
        for (int i = i1; i <= i2; i++)
            rgba[i] = p[i] << 4;

        p += 4;
        for (int j = c1; j < c2; j++, p += 4)
            for (int i = i1; i <= i2; i++)
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
    }

    for (int col = c1; col <= c2; col++) {
        p = result.scanLine(r2) + col * 4;
        for (int i = i1; i <= i2; i++)
            rgba[i] = p[i] << 4;

        p -= bpl;
        for (int j = r1; j < r2; j++, p -= bpl)
            for (int i = i1; i <= i2; i++)
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
    }

    for (int row = r1; row <= r2; row++) {
        p = result.scanLine(row) + c2 * 4;
        for (int i = i1; i <= i2; i++)
            rgba[i] = p[i] << 4;

        p -= 4;
        for (int j = c1; j < c2; j++, p -= 4)
            for (int i = i1; i <= i2; i++)
                p[i] = (rgba[i] += ((p[i] << 4) - rgba[i]) * alpha / 16) >> 4;
    }

    return result;
}

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
//Function to know if a goo is conteined in the ground
bool Ground::contains(Goo*goo){
    QPoint p=goo->getPPosition();
    int r=goo->getRadius();
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
