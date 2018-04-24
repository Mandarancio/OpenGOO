#pragma once

#include "OGLib/optional.h"

#include "wog_vobject.h"
#include "wog_exit.h"
#include "wog_pipe.h"
#include "wog_camera.h"

#include <QColor>

struct WOGBallInstance
{
    QString type;
    QString id;
    float x;
    float y;
    float angle;
    bool discovered;

    WOGBallInstance()
        : discovered(true)
    {
    }
};

struct WOGStrand
{
    QString gb1;
    QString gb2;
};

struct WOGSignpost : public WOGVObject
{
    QString name;
    QString image;
    QString text;
};

struct WOGLevel
{
    typedef QString Id;

    QColor textcolor;
    int ballsrequired;
    bool letterboxed;
    bool visualdebug;
    bool autobounds;
    bool texteffects;
    float timebugprobability;
    bool strandgeom;
    bool allowskip;
    Id music;
    std::list<WOGStrand> strand;
    std::list<WOGCamera> camera;
    std::list<WOGBallInstance> ball;
    std::list<WOGLevelExit> levelexit;
    std::list<WOGPipe> pipe;
    std::list<Id> loopsound;
    std::list<WOGSignpost> signpost;

    bool HasPipe() const
    {
        return !pipe.empty();
    }

    bool HasLevelExit() const
    {
        return !levelexit.empty();
    }

    const WOGLevelExit& GetLevelExit() const
    {
        return levelexit.front();
    }

    const WOGPipe& GetPipe() const
    {
        return pipe.front();
    }

    static std::unique_ptr<WOGLevel> Create()
    {
        return std::unique_ptr<WOGLevel>(new WOGLevel);
    }
};
