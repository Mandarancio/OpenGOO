#ifndef STICKYLINK_H
#define STICKYLINK_H

#include <QObject>
#include <Box2D/Box2D.h>

#include "ground.h"
#include "goo.h"

#include <QPoint>
#include <QPainter>

class StickyLink : public QObject
{
    Q_OBJECT
public:

    explicit StickyLink(Goo* goo,b2Body *ground, QPoint contactPoint,b2World *world, double strength=4,QObject *parent = 0);
    //Check the status of the joint
    void checkStatus();
    //Return the joint
    b2Joint* getJoint(){
        return joint;
    }
    //Set the joint [OBSOLETE]
    void setJoint(b2Joint* j){
        joint=static_cast<b2DistanceJoint*>(j);
    }
    //RETURN THE GOO
    Goo* getGoo(){
        return goo;
    }

    //DEBUG PAINT
    void paint(QPainter &p);

private:
    //Copy of the goo
    Goo * goo;
    //Physical joint
    b2DistanceJoint *joint;
    double strength;
signals:
    //SIGNAL TO ADVICE THE DESTRUCTION OF THE STICKY Joint!
    void destroySticky();
public slots:

};

#endif // STICKYLINK_H
