#ifndef THORN_H
#define THORN_H

#include "object.h"

#include <QObject>
#include <QPainter>
#include <Box2D/Box2D.h>

class Thorn : public Object
{
    Q_OBJECT
public:
    explicit Thorn(QPoint center,QList<QPoint>shape,b2World * world,QObject *parent = 0);
    QRect boundingRect(){
        return polygon.boundingRect();
    }
private:
    QPolygon polygon;
    void makeShape(QList<QPoint>points); //Function to make the shape starting from a QList of QPoint

signals:

public slots:
    void paint(QPainter &p);

};

#endif // THORN_H
