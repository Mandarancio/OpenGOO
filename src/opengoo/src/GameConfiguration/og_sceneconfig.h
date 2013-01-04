#ifndef OG_SCENECONFIG_H
#define OG_SCENECONFIG_H

#include "og_xmlconfig.h"

#include <QColor>
#include <QPointF>

typedef QPointF OGScale;
typedef QPointF OGPosition;

struct OGSceneLayer
{
    QString id;
    QString name;
    qreal depth;
    OGPosition position;
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
    OGPosition position;
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
    OGPosition center;
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
    OGPosition position;
    qreal pretick;
};

// Buttons

struct OGButton
{
    QString id;
    qreal depth;
    OGPosition position;
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

struct OGButtonGroup
{
    QString id;
    OGPosition osx;
    QList<OGButton> button;
};

// Static Geometry

struct OGCircle
{
    QString id;
    bool stat; // static
    QString tag;
    QString material;
    OGPosition position;
    qreal radius;
};

struct OGLine
{
    QString id;
    bool stat; // static
    QString tag;
    QString material;
    OGPosition anchor;
    OGPosition normal;
};

struct OGScene
{
    qreal minx;
    qreal miny;
    qreal maxx;
    qreal maxy;
    QColor backgroundcolor;
    QList<OGButton> button;
    QList<OGSceneLayer> sceneLayer;
    QList<OGLabel> label;
    QList<OGButtonGroup> buttongroup;
};

class OGSceneConfig : public OGXmlConfig
{
public:
    OGSceneConfig(const QString & filename);

    OGScene Parser();

private:
    OGButton CreateButton(const QDomElement & element);
    OGLabel CreateLabel(const QDomElement & element);
    OGSceneLayer CreateSceneLayer(const QDomElement & element);
};

#endif // OG_SCENECONFIG_H
