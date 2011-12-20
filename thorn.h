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
    explicit Thorn(QPoint p,int h,b2World * world,QObject *parent = 0);

private:
    QPolygon polygon;
signals:

public slots:
    void paint(QPainter &p);

};

#endif // THORN_H
