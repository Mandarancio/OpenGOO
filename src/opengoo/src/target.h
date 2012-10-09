#ifndef TARGET_H
#define TARGET_H

#include <QObject>
#include <QPoint>
#include <QPainter>
#include <Box2D/Box2D.h>

#include "goo.h"
#include "tools.h"

class Target : public QObject
{
    Q_OBJECT
public:
    explicit Target(QPoint position,int height,b2World * world,QObject *parent = 0);
    void checkTower(QList<Goo*> ps);
    void applyForces(QList<Goo*>ps);
    bool isCatched(){
        return catched;
    }
    b2Vec2 getVPosition(){
        return toVec(position);
    }
private:
    QPoint position;
    int h;
    bool catched;
    b2Body* body;
    void applyForce(Goo*goo);
    void applyImpulse(Goo*goo);
signals:
    void towerCatch();
    void towerLost();
    void gooCatched(Goo* goo);
public slots:
    void paint(QPainter &p);
};

#endif // TARGET_H
