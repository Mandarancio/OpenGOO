#ifndef COLLISIONLISTENER_H
#define COLLISIONLISTENER_H

#include <QObject>
#include <Box2D/Box2D.h>
#include "goo.h"


class CollisionListener : public QObject, public b2ContactListener
{
    Q_OBJECT
public:
    explicit CollisionListener(QObject *parent = 0);
    void PreSolve(b2Contact *contact, const b2Manifold *oldManifold);

signals:
    void destroyGOO(Goo * goo);
public slots:

};

#endif // COLLISIONLISTENER_H
