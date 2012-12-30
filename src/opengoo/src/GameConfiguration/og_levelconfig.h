#ifndef OG_LEVELCONFIG_H
#define OG_LEVELCONFIG_H

#include "og_xmlconfig.h"
#include <QPoint>
#include <QColor>

struct OGPoi
{
    QPointF position;
    qreal traveltime;
    qreal pause;
    qreal zoom;
};

struct OGCamera
{
    QString aspect;
    QPointF endpos;
    qreal endzoom;
    QList<OGPoi> poi;
};

struct OGMusic
{
    QString id;
};

typedef OGMusic OGLoopSound;

struct OGBallInstance
{
    QString type;
    QPointF position;
    QString id;
    qreal angle;
    bool discovered;
};

struct OGLevel
{
    qint32 ballsrequired;
    bool letterboxed;
    bool visualdebug;
    bool autobounds;
    QColor textcolor;
    bool texteffects;
    qreal timebugprobability;
    bool strandgeom;
    bool allowskip;
    QList<OGCamera> camera;
    OGMusic music;
    QList<OGBallInstance> ball;
};

class OGLevelConfig : public OGXmlConfig
{
public:
    OGLevelConfig(const QString & filename);

    OGLevel Parser();
};

#endif // OG_LEVELCONFIG_H
