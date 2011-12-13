#ifndef GROUND_H
#define GROUND_H

#include <QObject>
#include <QPainter>

#include <Box2D/Box2D.h>

class Ground : public QObject
{
    Q_OBJECT
public:
    explicit Ground(b2World *world,QPoint center,QList<QPoint>shape,QObject *parent = 0);
private:
    b2Body *body;
    QPolygon polShape;
    void makeShape(QList<QPoint>points);
signals:


public slots:
    void paint(QPainter &p);
};

#endif // GROUND_H
