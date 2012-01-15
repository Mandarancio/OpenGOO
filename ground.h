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
    b2Body * getBody(); //Get physical body
private:
    b2Body *body; //The physic body of the ground object
    QPolygon polShape;  //The polygonal shape
    void makeShape(QList<QPoint>points); //Function to make the shape starting from a QList of QPoint
signals:


public slots:
    void paint(QPainter &p); //Paint the ground
};

#endif // GROUND_H
