#ifndef LEVEL_H
#define LEVEL_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QTimerEvent>
#include <QMouseEvent>
#include <QKeyEvent>
#include <Box2D/Box2D.h>
#include <QTime>
#include <QPair>

#include "backgroundwidget.h"
#include "svglevelloader.h"
#include "balloongoo.h"
#include "background.h"
#include "stickylink.h"
#include "goo.h"
#include "ground.h"
#include "joint.h"
#include "target.h"
#include "menu.h"
#include "object.h"
#include "qb2draw.h"


struct Direction {
    bool key;
    bool right,left;
    bool up,down;
};

//This is the Scene Widget
//It initialize evrithing
//The world (b2World) object is here
class Level : public QWidget
{
    Q_OBJECT
public:
    explicit Level(QString level,BackGroundWidget *bg,QWidget *parent = 0); //Geometry is needed to have the display dimension information, level is the level to load
    ~Level(){
        clean();
    }
    //Function to start the level;
    bool startLevel();


private:
    float scale;
    //LOADER
    SvgLevelLoader * loader;
    //PROPERTY
    //Graphic
    QPoint center;          //Center of the display, is used to have a more human coordinate system
    QPoint translation;     //For the level translation
    bool onMenu;
    bool mooving;
    Menu* menu;
    //Physic
    b2World *world;         //world object of the box2d lib
    float step;             //it say how much long is a step
    //Game
    QList <Ground *>ground;         //Ground object
    QList <BackGround*> background; //background objects

    QList<Goo*> goos;       //All the goos!
    QList<Goo*> goosToDestroy;  //GOOs to be destroyed the next update!
//    QList<BalloonGoo*> ballGoos; //Balloon goos

    QList<Joint*> joints;   //All the joints!
    QList<Joint*> jointsToDestroy; //Joints to be destroyed the next update!

    //STICKY VARIABLE
    //LIST OF THE STICKY TO BE CREATED
    QList< QPair<Goo*,QPoint> > stickyToCreate;
    //DEBUG LIST
    QList<StickyLink*> stickys;

    QList<Object*> objects;
    Target* target;         //The target object
    bool drag;              //If the player is dragging some goo
    Goo* dragged;           //The goo that is under the mouse
    Goo* selected;          //The selected Goo
    QPoint stopPosition;    //If mouse is in the ground
    //Mouse
    b2Vec2 mousePos;        //Mouse current position (is needed to calculate the speed of the mouse
    b2Vec2 mouseSpeed;      //Mouse speed (so when release the dragged goo, he take the mouse speed)
    QPoint absoluteMousePos;
    //Level
    QString name;           //Level name
    int goal;               //Number of goo to collect to win
    int points;             //Number of goo collected
    bool catched;           //If the tower of Goos is catched
    QRect limit;            //Translation limit
    Direction dir;          //Translation direction
    QList<Goo*> possibility;  //To show the possible joints on the dragged goo

    //TIMER & COUNTER
    int showJointTimer;

    int positionTimer;
    int draggTimer;


    double time;

    //DEBUGGER PAINTER!
    QB2Draw * debugPainter;

    bool mute;  // if true turn off music, if false turn on music

    //METHOD
    BackGroundWidget * backGroundWidget; //This is a widget to draw the semi-static ground and background.

    //INITIALIZE FUNCTION
    void initialize();
    //CLEAN FUNCTION
    void clean();

    Joint* overJoint(Goo* goo);

    Goo* getGooAt(QPoint p);    //Funciton to get (if any) a goo in a point +/- the radius of the goo

    Goo* getNearest(QPoint p,QList<Goo*> l);

    bool groundContains(Goo* goo);
    bool groundContains(QPoint p, int radius);

    //Function to translate the gui
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void moveOf(QPoint dP);

    bool makeJoint(Goo*a,Goo*b);    //Function to create a single joint between two goos
    QList<Goo*> possibleJoints(QPoint p); //Function to show all the possible joint from a point
    bool createJoints(QPoint p);    //Function to create


    void anchorToJoint(Goo* goo,Joint * j);

    //Funcion to draw background, score and win
    void paintScore(QPainter &p);
    void paintWin(QPainter &p);

    //paint an arrow for the target
    void paintTargetArrow(QPainter &p);

    //MENU BUTTON FOR TOUCHSCREEN
    void paintButton(QPainter &p);
    void clickButton(QPoint p);

    //ZOOM FUNCTION
    bool zoom(float d);
    
    //MUTE BUTTON
    void paintMute(QPainter &p);
    void clickMute(QPoint p);

    //Sound
    SoundSystem* sSystem;
    
protected:
    //Timer event function here the world is updated and after the scene is redr
    void timerEvent(QTimerEvent *);
    //Function that draw evrithing in the scene
    void paintEvent(QPaintEvent *);
    //Key release function (esc: to exit, up/left/down/right to move the scene
    void keyReleaseEvent(QKeyEvent *);
    //Key press function;
    void keyPressEvent(QKeyEvent *);
    //Mouse moove event
    void mouseMoveEvent(QMouseEvent *);
    //Press mouse event
    void mousePressEvent(QMouseEvent *);
    //Release mouse event
    void mouseReleaseEvent(QMouseEvent *);
    //Weel event for zooming
    void wheelEvent(QWheelEvent *);
    //Resize window event
    void resizeEvent(QResizeEvent *);

signals:
    void closing();                     //When level is to close for some error reason
    void eventBackToMainMenu();
public slots:
private slots:
    //LEVEL LOADER SLOTS:
    void setGoal(int goal);
    void setName(QString name){
        this->name=name;
    }
    void setLimit(QRect limit);
    void setTarget(QPoint target);
    void setGround(QPoint gCenter,QList<QPoint> gList);
    void setGoo(QPoint center,int id, int type=0);
    void setStartArea(int n,QRect area,int type=0);
    void setJoint(Goo *a, Goo *b);
    //To scale at the right dimension
    void setLevelGeometry(QSize size);
    //create thorn
    void setThorns(QPoint center, QList<QPoint> list);

    //GAME SLOT
    //Functions to create and destroy sticky joints
    void createSticky(QPoint p);
    void destroySticky();
    //Functions for wakeup poor sleeping goos
    void checkForNeighbors(QPoint p);
    //Slot to stop dragging a goo (is used when the user try to drag a goo in some not permitted position)
    void stopDragging();
    //Slot to stop a dragged goo in a Point
   //OBSOLETE void stopGoo(QPoint p);
    //Slot add a shape to background.
    void addBGShape(int level,QPolygon poly,QColor color);

    void destroyJoint(Joint * joint);   //Destroy a joint
    void destroyGOO();                  //Destroy a GOO!
    void gooCatched(Goo * goo);         //Target catch a goo
    void giveTarget(Goo* previous);     //Function to give a jointed goo to follow to a free goo
    void destroyJoint(Goo*a,Goo*b);
    void towerCatched();                //Actions to do when the tower is near the target
    void towerLost();                   //Actions to do when the tower in no more near the target

    //LEVEL SLOTS:
    void resume();                      //Close the menu
    void restart();                     //Restart the level
    void closeAll(){                    //Close the game
        emit this->closing();
    }
    void backToMainMenu(){              //Return to Main Menu
        emit this->eventBackToMainMenu();
    }
};

#endif // LEVEL_H
