#pragma once

#include "wog_exit.h"
#include "wog_pipe.h"

#include <QColor>

struct WOGPoi
{
    QPointF position;
    float traveltime;
    float pause;
    float zoom;
};

struct WOGCamera
{
    enum Aspect
    {
        Unknown,
        Normal,
        WideScreen
    };

    QPointF endpos;
    float endzoom;
    Aspect aspect;
    QVector<WOGPoi> poi;
};

struct WOGMusic { QString id; };

typedef WOGMusic WOGLoopSound;

struct WOGBallInstance
{
    QString type;
    QString id;
    float x;
    float y;
    float angle;
    bool discovered;
};

struct WOGStrand
{
    QString gb1;
    QString gb2;
};

struct WOGLevel
{
    QColor textcolor;
    int ballsrequired;
    bool letterboxed;
    bool visualdebug;
    bool autobounds;
    bool texteffects;
    float timebugprobability;
    bool strandgeom;
    bool allowskip;
    WOGMusic music;
    QVector<WOGStrand> strand;
    QVector<WOGCamera> camera;
    QVector<WOGBallInstance> ball;
    std::pair<WOGLevelExit, bool> levelexit;
    std::pair<WOGPipe, bool> pipe;
};
