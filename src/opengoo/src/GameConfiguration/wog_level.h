#ifndef WOG_LEVEL_H
#define WOG_LEVEL_H

#include <QPointF>
#include <QString>
#include <QList>
#include <QColor>

struct WOGPoi
{
    QPointF position;
    qreal traveltime;
    qreal pause;
    qreal zoom;
};

struct WOGCamera
{
    QString aspect;
    QPointF endpos;
    qreal endzoom;
    QList<WOGPoi*> poi;

    ~WOGCamera();
};

struct WOGMusic { QString id; };

typedef WOGMusic WOGLoopSound;

struct WOGBallInstance
{
    enum Type {COMMON, DISTANT};

    Type type;
    QPointF position;
    QString id;
    qreal angle;
    bool discovered;
};

struct WOGLevelExit
{
    QString id;
    QPointF pos;
    int radius;
    QString filter;
};

struct WOGPipe
{
    QString id;
    qreal depth;
    QList<QPointF> vertex;
};

struct WOGStrand
{
    QString bg1_;
    QString bg2_;
};


struct WOGLevel
{
    int ballsrequired;
    bool letterboxed;
    bool visualdebug;
    bool autobounds;
    QColor textcolor;
    bool texteffects;
    qreal timebugprobability;
    bool strandgeom;
    bool allowskip;
    QList<WOGCamera*> camera;
    WOGMusic music;
    QList<WOGBallInstance*> ball;
    WOGLevelExit* levelexit;
    QList<WOGStrand*> strand;
    WOGPipe pipe;

    WOGLevel() : levelexit(0) { }
    ~WOGLevel();
};

#endif // WOG_LEVEL_H
