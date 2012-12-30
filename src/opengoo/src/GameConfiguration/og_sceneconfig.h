#ifndef OG_SCENECONFIG_H
#define OG_SCENECONFIG_H

#include "og_xmlconfig.h"

#include <QColor>
#include <QPointF>

struct OGScale
{
    qreal x;
    qreal y;
};

struct OGSceneLayer
{
    QString id;
    QString name;
    qreal depth;
    QPoint position;
    OGScale scale;
    qreal rotation;
    qreal alpha;
    QColor colorize;
    QString image;
    QString anim;
    qreal animspeed;
};

struct OGLabel
{
    QString id;
    qreal depth;
    QPointF position;
    QString align;
    qreal rotation;
    qreal scale;
    bool overlay;
    bool screenspace;
    QString font;
    QString text;
};

// ForceFields

struct OGRadialForceField
{
    QString id;
    QString type;
    QPointF center;
    qreal radius;
    qreal forceatcenter;
    qreal forceatedge;
    qreal dampeningfactor;
    bool antigrav;
    bool geomonly;
    bool enabled;
};

struct OGLinearForceField
{
    QString type;
    qreal force;
    qreal dampeningfactor;
    bool antigrav;
    bool geomonly;
};

struct OGParticle
{
    QString effect;
    qreal depth;
    QPointF position;
    qreal pretick;
};

// Buttons

struct OGButtonGroup
{
    QString id;
    QPointF osx;
};

struct OGButton
{
    QString id;
    qreal depth;
    QPointF position;
    OGScale scale;
    qreal rotation;
    qreal alpha;
    QColor colorize;
    QString up;
    QString over;
    QString disabled;
    QString onclick;
    QString onmouseenter;
    QString onmouseexit;
};

// Static Geometry

struct OGCircle
{
    QString id;
    bool stat; // static
    QString tag;
    QString material;
    QPointF position;
    qreal radius;
};

struct OGLine
{
    QString id;
    bool stat; // static
    QString tag;
    QString material;
    QPointF anchor;
    QPointF normal;
};

struct OGScene
{
    qreal minx;
    qreal miny;
    qreal maxx;
    qreal maxy;
    QColor backgroundcolor;
    QList <OGSceneLayer> sceneLayers;
    QList <OGLabel> labels;
};

class OGSceneConfig : public OGXmlConfig
{
public:
    OGSceneConfig(const QString & filename);

    OGScene Parser();
};

#endif // OG_SCENECONFIG_H
