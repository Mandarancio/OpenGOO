#include "stickygoo.h"

StickyGoo::StickyGoo(b2World*world,QPoint point,int radius,QObject *parent) :
    RemovableGoo(world,point,radius,parent)
{
    color=QColor(188,211,95);
    secondaryColor=QColor(188,211,95);
    stickness=100;
    type=STICKY;
}
