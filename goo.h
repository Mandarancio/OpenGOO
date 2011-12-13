#ifndef GOO_H
#define GOO_H

#include <QObject>
#include <QPainter>
#include <QList>

#include <Box2D/Box2D.h>

struct dragInfo{
    b2Vec2 speed;
    float aForce;
    float gScale;
};

class Goo : public QObject
{
    Q_OBJECT
public:
    explicit Goo(int radius=15,QObject *parent = 0);

    int nJoints();
    int getMaxJoints();
    int getRadius();
    int getGuestNumber();

    bool newGuest();
    bool removeGuest();

    void drag();
    void drop();
    void drop(b2Vec2 speed);

    bool isDragable();
    bool isMoovable();
    bool isFalling();
    bool hasJoint();
    bool canHaveJoint();
    bool isLinked(Goo* goo);

    virtual bool createLink(Goo* goo);
    virtual bool destroyLink(Goo* goo);

    virtual void contactGround();
    void setTarget(Goo* goo);

    void move(QPoint p);
    b2Body* getBody();
    b2Vec2 getVPosition();
    QPoint getPPosition();

    QList <Goo*> getLinks();

private:
    QList <Goo*> links;
    int radius;
    dragInfo info;

protected:
    b2Body* body;
    bool dragable;
    bool moovable;
    bool falling;
    bool following;
    bool dragging;
    int minJoints,maxJoints;
    int guestN;

    Goo* target,*prevTarget;

    void moveToTarget();
    void stopFollow();
signals:
    void nextTargetPlease(Goo* previous);
    void loseLink(Goo* goo);

private slots:
    void checkForConnection(Goo* goo);

public slots:
    virtual void paint(QPainter &p);

};

#endif // GOO_H
