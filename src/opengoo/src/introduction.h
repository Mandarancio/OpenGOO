#ifndef INTRODUCTION_H
#define INTRODUCTION_H

#include <QWidget>
#include <Box2D/Box2D.h>
#include <QTimerEvent>
#include <QPaintEvent>
#include <QTime>

#include "svglevelloader.h"
#include "qb2draw.h"

#include "goo.h"
#include "ground.h"

class Introduction : public QWidget
{
    Q_OBJECT
public:
    explicit Introduction(QWidget *parent = 0);
    ~Introduction();
    
private:
    QTime itime;

    double time;
    float scale;
    float step;
    float endTime;

    b2World * world;

    QList<Goo*> goos;
    QList<Ground*> ground;


    //LOADER
    SvgLevelLoader * loader;


    //DEBUGGER PAINTER!
    QB2Draw * debugPainter;
    int nFrame;

    //SOUND
    SoundSystem* sSystem;

protected:
    //Timer event function here the world is updated and after the scene is redr
    void timerEvent(QTimerEvent *);
    //Function that draw evrithing in the scene
    void paintEvent(QPaintEvent *);
    //Resize Event
    void resizeEvent(QResizeEvent *);
    //Key event (for skip intro)
    void keyPressEvent(QKeyEvent *);

signals:
    void introEnd(); //!Emitted when the intro has finished.

private slots:
    void setGround(QPoint gCenter,QList<QPoint> gList);
    void setGoo(QPoint center, int id=0, int type=0);
    void setStartArea(int n,QRect area,int type=0);
public slots:

};

#endif // INTRODUCTION_H
