#ifndef STICKYLINK_H
#define STICKYLINK_H

#include <QObject>
#include <Box2D/Box2D.h>

#include "ground.h"
#include "goo.h"

#include <QPoint>

class StickyLink : public QObject
{
    Q_OBJECT
public:
    explicit StickyLink(Goo* goo,Ground *ground, QPoint contactPoint,b2World *world, int strength=2000,QObject *parent = 0);
    void checkStatus();
    b2Joint* getJoint();
private:
    b2DistanceJoint *joint;
    int strength;
signals:
    void destroySticky();
public slots:

};

#endif // STICKYLINK_H
