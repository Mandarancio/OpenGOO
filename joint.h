#ifndef JOINT_H
#define JOINT_H

#include <QObject>
#include <Box2D/Box2D.h>
#include <QPainter>

#include "goo.h"

class Joint : public QObject
{
    Q_OBJECT
public:
    explicit Joint(Goo* a,Goo* b, b2World * world,QObject *parent = 0);
    void paint(QPainter &p);
    b2Joint* getJoint();
    bool has(Goo*a,Goo*b);
protected:
    b2DistanceJoint* joint;
    Goo *a,*b;
    virtual void status();
signals:
    void destroyJoint(Joint *j);
public slots:

};

#endif // JOINT_H
