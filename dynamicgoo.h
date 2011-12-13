#ifndef DYNAMICGOO_H
#define DYNAMICGOO_H

#include "goo.h"

class DynamicGoo : public Goo
{
public:
    DynamicGoo(b2World * world,QPoint p=QPoint(0,0),int radius=15,QObject *parent = 0);
};

#endif // DYNAMICGOO_H
