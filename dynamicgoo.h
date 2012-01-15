#ifndef DYNAMICGOO_H
#define DYNAMICGOO_H

#include "goo.h"

class DynamicGoo : public Goo
{
public:
    DynamicGoo(b2World * world,QPoint p=QPoint(0,0),int radius=15,QObject *parent = 0);
    void catched(); //if the tower catch the target
    void lost(); //if is the tower lost the target
private:
    b2World * world;// copy of the world
    int speed; //Speed
protected:
    void moveToTarget(); //move to the target
public slots:
    void paint(QPainter &p); //Draw the goo
};

#endif // DYNAMICGOO_H
