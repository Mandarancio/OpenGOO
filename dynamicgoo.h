#ifndef DYNAMICGOO_H
#define DYNAMICGOO_H

#include "goo.h"

class DynamicGoo : public Goo
{
public:
    DynamicGoo(b2World * world,QPoint p=QPoint(0,0),int radius=15,QObject *parent = 0);
    void catched();
    void lost();
private:
    int speed;
protected:
    void moveToTarget();
public slots:
    void paint(QPainter &p);
};

#endif // DYNAMICGOO_H
