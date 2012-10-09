#ifndef ROPEJOINT_H
#define ROPEJOINT_H

#include "joint.h"
#include <Box2D/Box2D.h>
#include <QList>

class RopeJoint : public Joint
{
    Q_OBJECT
public:
    explicit RopeJoint(Goo* a,Goo* b, b2World * world,QObject *parent = 0);
    void status();
    ~RopeJoint();
private:
    QList <b2Joint*> joints;
    QList <b2Body*> bodies;
    b2World* world;
protected:
    void initialize(b2World *world);
    void paint(QPainter &p);

public slots:
};

#endif // ROPEJOINT_H
