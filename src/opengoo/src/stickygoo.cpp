#include "stickygoo.h"

StickyGoo::StickyGoo(b2World*world,QPoint point,int radius,QObject *parent) :
    RemovableGoo(world,point,radius,parent)
{

    color=QColor(255,153,85);
    secondaryColor=color;
    stickness=100;
    type=STICKY;
}
