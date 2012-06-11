#ifndef DYNAMICGOO_H
#define DYNAMICGOO_H

#include "goo.h"

#include <QObject>

/*
  this is the most standard goo, if is free is moovable and dragable but
  when is jointed to other ones is not more possible darg or moove it..
  If the joint is broke for same razon is possible to reuse it again.
  */
class DynamicGoo :public Goo
{
    Q_OBJECT
public:
    DynamicGoo(b2World * world,QPoint p=QPoint(0,0),int radius=15,QObject *parent = 0);
    void catched(); //if the tower catch the target
    void lost(); //if is the tower lost the target
    void contactGround();  //collision between goo and ground
    void contactGround(QPoint p);//collision between goo and ground  in the point p(create a sticky joint!)
    void unstick(); //To unstick the goo
    //Function that return if the goo is now draggable (if is linked is not)
    bool isDragable();
    QColor getColor();
private:
    b2World * world;// copy of the world
    int speed; //Speed
    bool sticked; //flag to know if is sticked to the ground

protected:
    void moveToTarget(); //move to the target
    QColor color; //Color for recognize type of dynamic goo
    QColor secondaryColor; //For recognize secondary option
    bool stickable; //Flag to know if is stickable
    int counter; //ANIMATION COUNTER
    int rx,ry; //ANIMATION COORDINATE
    bool eye;
    int eyeSizeR,eyeSizeL;
private slots:

public slots:
    void paint(QPainter &p); //Draw the goo
    //Draw debug data
    void paintDebug(QPainter &p);
    //rutines called when previus status was spleep and a neighbor is found!
    void neighborsFound();


};

#endif // DYNAMICGOO_H
