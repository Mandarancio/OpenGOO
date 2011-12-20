#ifndef THORN_H
#define THORN_H

#include <QObject>
#include <QPainter>
#include <Box2D/Box2D.h>

class Thorn : public QObject
{
    Q_OBJECT
public:
    explicit Thorn(QPoint p,int h,float a,b2World * world,QObject *parent = 0);

private:
    b2Body *body;
    QPolygon polygon;
    float angle;
signals:

public slots:
    void paint(QPainter &p);

};

#endif // THORN_H
