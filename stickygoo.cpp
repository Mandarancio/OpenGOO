#include "stickygoo.h"

StickyGoo::StickyGoo(b2World*world,QPoint point,int radius,QObject *parent) :
    RemovableGoo(world,point,radius,parent)
{
    color=Qt::blue;
    secondaryColor=Qt::blue;
    stickness=100;
    type=STICKY;
}
