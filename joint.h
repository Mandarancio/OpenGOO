#ifndef JOINT_H
#define JOINT_H

#include <QObject>
#include <Box2D/Box2D.h>
#include <QPainter>

#include "goo.h"
enum JointType {NORMAL, ROPE};
class Joint : public QObject
{
    Q_OBJECT
public:
    explicit Joint(Goo* a,Goo* b, b2World * world,bool child=false,QObject *parent = 0); //cunstractor: the two goo to link and the phisical world
    virtual void paint(QPainter &p); //Paint function
    //DEBUG PAINTER
    void paintDebug(QPainter &p);
    b2Joint* getJoint(); //To have acces at the material b2joint
    bool has(Goo*a,Goo*b); //To check if this joint link this two GOO
    JointType getType();
protected:
    JointType type;
    virtual void initialize(b2World * world);
    b2Joint* joint; //The phisical joint
    Goo *a,*b; //the two linked goo
    virtual void status(); // function to check the status of the joint
signals:
    void destroyJoint(Joint *j); //SIGNAL FOR DESTROY THIS JOINT
public slots:
};

#endif // JOINT_H