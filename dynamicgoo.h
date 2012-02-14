#ifndef DYNAMICGOO_H
#define DYNAMICGOO_H

#include "goo.h"

#include <QObject>

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

    bool isDragable();
private:
    b2World * world;// copy of the world
    int speed; //Speed
    bool sticked; //flag to know if is sticked to the ground
protected:
    void moveToTarget(); //move to the target
private slots:

public slots:
    void paint(QPainter &p); //Draw the goo
    //Draw debug data
    void paintDebug(QPainter &p);
    //rutines called when previus status was spleep and a neighbor is found!
    void neighborsFound();

signals:
    void createSticky(QPoint p);
};

#endif // DYNAMICGOO_H
