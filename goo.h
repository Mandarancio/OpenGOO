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

    virtual bool isDragable();
    bool isMoovable();
    bool isFalling();
    bool isDragging();
    bool hasJoint();
    bool canHaveJoint();
    bool isLinked(Goo* goo);
    bool isOnGround();
    bool isSleeping();

    virtual bool createLink(Goo* goo);
    virtual bool destroyLink(Goo* goo);

    virtual void contactGround()=0; //contact with the ground
    virtual void contactGround(QPoint p)=0; //contact with the ground in the point p
    virtual void destroyThis(); //destroy this goo
    void setTarget(Goo* goo);

    void move(QPoint p);

    void jumpTo(QPoint p);

    virtual void catched();
    virtual void lost();


    b2Body* getBody();
    b2Vec2 getVPosition();
    QPoint getPPosition();

    QList <Goo*> getLinks();
    Goo* getPrevious();
private:
    QList <Goo*> links;
    int radius;
    dragInfo info;
protected:
    b2Body* body; //physical body
    //FLAGS
    bool dragable; // If the user can drag it
    bool moovable; // If is moovable
    bool falling; //If is falling down
    bool following;//If is following another goo
    bool dragging; //If the user is courrently dragging that
    bool onGround; //If the goo is on the ground (and inactive)
    //If the goo is sleeping
    bool sleeping;
    //Propierties
    QPoint groundPoint;
    int minJoints,maxJoints;
    int guestN;

    Goo* target,*prevTarget;


    void moveToTarget(); //Move the goos through the joints
    void stopFollow();
    void fallDown();
signals:
    void nextTargetPlease(Goo* previous);
    void loseLink(Goo* goo);
    void destroyGoo();
    void destroyJoint(Goo* a,Goo*b);
    void checkForNeighbors(QPoint p);
private slots:
    void checkForConnection(Goo* goo);
    virtual void deleteSticky(){};

public slots:
    virtual void paint(QPainter &p)=0;//Draw the goo
    //PaintDebug data
    virtual void paintDebug(QPainter &p)=0;
    virtual void neighborsFound();
};

#endif // GOO_H
