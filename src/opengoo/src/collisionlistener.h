#ifndef COLLISIONLISTENER_H
#define COLLISIONLISTENER_H

#include <QObject>
#include <Box2D/Box2D.h>
#include "goo.h"


class CollisionListener : public QObject, public b2ContactListener
{
    Q_OBJECT
public:
    explicit CollisionListener(QObject *parent = 0) :
        QObject(parent)
    {
        //Nothing to do here (for now)
    }
    void PreSolve(b2Contact *contact, const b2Manifold *oldManifold); //Pre solve of the collision
    void PostSolve(b2Contact *contact, const b2ContactImpulse *impulse);

signals:
    void destroyGOO(Goo * goo); //signal to destroy a goo (Ex: a goo collide with a thorn)
    void stopGOO(QPoint p);
public slots:

};

#endif // COLLISIONLISTENER_H
