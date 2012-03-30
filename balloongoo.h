#ifndef BALLOONGOO_H
#define BALLOONGOO_H
#include "goo.h"

#include <Box2D/Box2D.h>

class BalloonGoo : public Goo
{
public:
    BalloonGoo(b2World* world, QPoint p=QPoint(0,0),int radius=15,QObject *parent = 0);
    void contactGround();
    void contactGround(QPoint p);
public slots:
    void paint(QPainter &p);
    void paintDebug(QPainter &p);
};

#endif // BALLOONGOO_H
