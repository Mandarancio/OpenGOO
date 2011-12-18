#ifndef LEVEL_H
#define LEVEL_H

//#include <QWidget> <--To use without opengl
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
#include "menu.h"

//This is the Scene Widget
//It initialize evrithing
//The world (b2World) object is here
class Level : public QGLWidget //QWidget <--To use without openGL
{
    Q_OBJECT
public:
    explicit Level(QRect geometry,QString level,QWidget *parent = 0); //Geometry is needed to have the display dimension information, level is the level to load
    ~Level();

private:
    //PROPERTY
    //Graphic
    QPoint center;          //Center of the display, is used to have a more human coordinate system
    QPoint translation;     //For the level translation
    bool onMenu;
    Menu* menu;
    //Physic
    b2World *world;         //world object of the box2d lib
    float step;             //it say how much long is a step
    //Game
    Ground *ground;         //Ground object
    QList<Goo*> goos;       //All the goos!
    QList<Joint*> joints;   //All the joints!
    Target* target;         //The target object
    bool drag;              //If the player is dragging some goo
    Goo* dragged;           //The goo that is under the mouse
    //Mouse
    b2Vec2 mousePos;        //Mouse current position (is needed to calculate the speed of the mouse
    b2Vec2 mouseSpeed;      //Mouse speed (so when release the dragged goo, he take the mouse speed)
    //Level
    QString name;           //Level name
    int goal;               //Number of goo to collect to win
    int points;             //Number of goo collected
    bool catched;           //If the tower of Goos is catched
    int nBalls;             //Number of dynamic goo in the level!
    QRect startArea;        //Where the dynamic goos are created
    b2Vec2 startForce;      //If any, is the start force
    QRect limit;            //Translation limit
    QList<QPoint> possibility;  //To show the possible joints on the dragged goo
    //METHOD
    //Level

    void readLevel(QString path);   //Function to load the level from file
    bool parseString(QString string);   //Split a line of the file in the two component the tag and the real information
    bool parseInfo(QString tag,QString info); //Recognize the tag and use the info

    void createBalls(); //To initialize the dynamic goos

    Goo* getGooAt(QPoint p);    //Funciton to get (if any) a goo in a point +/- the radius of the goo

    //Function to translate the gui
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();

    bool makeJoint(Goo*a,Goo*b);    //Function to create a single joint between two goos
    QList<QPoint> possibleJoints(QPoint p); //Function to show all the possible joint from a point
    bool createJoints(QPoint p);    //Function to create

    //Funcion to draw background, score and win
    void paintBg(QPainter &p);
    void paintScore(QPainter &p);
    void paintWin(QPainter &p);

protected:
    void timerEvent(QTimerEvent *); //Timer event function here the world is updated and after the scene is redr
    void paintEvent(QPaintEvent *); //Function that draw evrithing in the scene
    void keyReleaseEvent(QKeyEvent *);  //Key release function (esc: to exit, up/left/down/right to move the scene
    void mouseMoveEvent(QMouseEvent *); //Mouse moove event
    void mousePressEvent(QMouseEvent *);    //Press mouse event
    void mouseReleaseEvent(QMouseEvent *);  //Release mouse event
signals:
    void closing(); //When level is to close for some error reason
public slots:
private slots:
    void destroyJoint(Joint * joint);   //Destroy a joint
    void gooCatched(Goo * goo);         //Target catch a goo
    void giveTarget(Goo* previous);     //Function to give a jointed goo to follow to a free goo
    void towerCatched();                //Actions to do when the tower is near the target
    void towerLost();                   //Actions to do when the tower in no more near the target
};

#endif // LEVEL_H
