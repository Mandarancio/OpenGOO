#include "removablegoo.h"
#include "tools.h"

//The constructor is not different at the Dynamic goo constructor
RemovableGoo::RemovableGoo(b2World *world, QPoint point, int radius, QObject *parent) :
    DynamicGoo(world,point,radius,parent)
{
}

//The only difference with dynamic goo is that the joint status is ignored.
bool RemovableGoo::isDragable(){
   return !isSleeping();
}

//Change the color of the paint debug for recognize it from Dynamic goo
void RemovableGoo::paintDebug(QPainter &p){
    //set the pen green if is jointed or white if is free and not sleeping
    p.setPen((hasJoint()? Qt::blue : Qt::lightGray));
    //set pen yellow if the goo is dragged from the user
    if (isDragging()) p.setPen(Qt::yellow);
    //set pen red if is sleeping
    if (isSleeping()) p.setPen(Qt::darkRed);
    //the brush is transparent
    p.setBrush(Qt::transparent);
    //draw the contourn
    p.drawEllipse(toPoint(body->GetPosition()),getRadius(),getRadius());
    //if has joint draw the number of joint of it inside him.
    if (hasJoint()){
        p.drawText(QPoint(getPPosition().x()-5,getPPosition().y()+5),QString::number(this->nJoints()));
    }
    //if not and is free and is mooving to reach a target draw the direction
    else if (target!=NULL){
        //save the position
        p.save();
        //translate the painter at the center of the goo
        p.translate(getPPosition());
        //rotate the painter of the angle between the goo and his target
        p.rotate(atan2(target->getPPosition().y()-getPPosition().y(),target->getPPosition().x()-getPPosition().x())*180.0/3.141628);
        //draw a line in this direction of 40 px
        p.drawLine(0,0,40,0);
        //translate the painter at the end of the line
        p.translate(40,0);
        //rotate of 45° degree
        p.rotate(45);
        //draw a line of 5 (this is one side of the arrow)
        p.drawLine(0,0,-5,0);
        //rotate to -45° degree
        p.rotate(270);
        //draw the other side of the arrow
        p.drawLine(0,0,-5,0);
        //restore the previous position
        p.restore();
    }
}
