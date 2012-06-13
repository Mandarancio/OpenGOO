#ifndef TOOLS_H
#define TOOLS_H

#include <QPoint>
#include <QRect>
#include <QSize>
#include <QString>
#include <QStringList>
#include <QList>
#include <QPolygon>
#include <Box2D/Box2D.h>

//Conversion for qt/box2d
inline b2Vec2 toVec(QPoint p){ //Convert a QPoint in a b2Vec2
    return b2Vec2(p.x()/10.0,p.y()/10.0);
}
inline QPoint toPoint(b2Vec2 v){ //Convert a b2Vec2 in a QPoint
    return QPoint(qRound(v.x*10.0),qRound(v.y*10.0));
}

QPolygon toPoly(QList<QPoint> ps); //Convert a QList of QPoint in a QPolygon
QPolygon toPoly(QList<QPoint> ps,QPoint center); //Convert a QList of QPoint in a QPolygon transated of *center*



//Level file parse function

QPoint pPoint(QString s, bool &ok,int w=1000,int h=1000); //Parse a string "X Y" in a QPoint(x,y) with cordinate scaled of 1000/w and 1000/h
QSize pSize(QString s, bool &ok); //parse a string "W H" in a QSize(w,h)
QRect pRect(QString s, bool &ok, int w=1000,int h=1000); //Parse a string "X Y,W H" in a QRect(x,y,w,h) with cordinade scaled of 1000/w and 1000/h
QList <QPoint> pList(QString s, bool&ok,int w=1000,int h=1000, int min=3); //Parse a string "X0 Y0,X1 Y1,...,XN YN" in a QList of QPoint scaled of 1000/w and 1000/h


QString time2string(int time);

#endif // TOOLS_H
