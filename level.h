#ifndef LEVEL_H
#define LEVEL_H

#include <QGLWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QTimerEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <Box2D/Box2D.h>


#include "goo.h"
#include "ground.h"
#include "joint.h"
#include "target.h"

class Level : public QGLWidget
{
    Q_OBJECT
public:
    explicit Level(QRect geometry,QString level,QWidget *parent = 0);
    ~Level();

private:
    //PROPERTY
    //Graphic
    QPoint center;
    QPoint translation;
    //Physic
    b2World *world;
    float step;
    //Game
    Ground *ground;
    QList<Goo*> goos;
    QList<Joint*> joints;
    Goo* dragged;
    Target* target;
    bool drag;
    //Mouse
    b2Vec2 mousePos;
    b2Vec2 mouseSpeed;
    //Level
    QString name;
    int goal;
    int points;
    bool catched;
    int nBalls;
    QRect startArea;
    b2Vec2 startForce;
    QRect limit;
    QList<QPoint> possibility;
    //METHOD
    //Level

    void readLevel(QString path);
    bool parseString(QString string);
    bool parseInfo(QString tag,QString info);

    void createBalls();

    Goo* getGooAt(QPoint p);

    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();

    bool makeJoint(Goo*a,Goo*b);
    QList<QPoint> possibleJoints(QPoint p);
    bool createJoints(QPoint p);

    void paintBg(QPainter &p);
    void paintScore(QPainter &p);
    void paintWin(QPainter &p);

protected:
    void timerEvent(QTimerEvent *);
    void paintEvent(QPaintEvent *);
    void keyReleaseEvent(QKeyEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
signals:
    void closing(); //When level is to close for some error reason
public slots:
private slots:
    void destroyJoint(Joint * joint);
    void gooCatched(Goo * goo);
    void giveTarget(Goo* previous);
    void towerCatched();
    void towerLost();
};

#endif // LEVEL_H
