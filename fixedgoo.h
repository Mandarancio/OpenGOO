#ifndef FIXEDGOO_H
#define FIXEDGOO_H

#include "goo.h"

class FixedGoo : public Goo
{
public:
    FixedGoo(b2World* world, QPoint p=QPoint(0,0),int radius=15,QObject *parent = 0);
    void contactGround();
    void contactGround(QPoint p);
public slots:
    void paint(QPainter &p);
};

#endif // FIXEDGOO_H
