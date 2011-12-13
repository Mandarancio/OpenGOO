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
b2Vec2 toVec(QPoint p);
QPoint toPoint(b2Vec2 v);

b2Vec2* toVecs(QList<QPoint> ps);
QPolygon toPoly(QList<QPoint> ps);
QPolygon toPoly(QList<QPoint> ps,QPoint center);



//Level file parse function

QPoint pPoint(QString s, bool &ok,int w=1000,int h=1000);
QSize pSize(QString s, bool &ok);
QRect pRect(QString s, bool &ok, int w=1000,int h=1000);
QList <QPoint> pList(QString s, bool&ok,int w=1000,int h=1000, int min=3);


#endif // TOOLS_H
