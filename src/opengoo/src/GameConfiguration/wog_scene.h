#ifndef WOG_SCENE_H
#define WOG_SCENE_H

#include "wog_pobject.h"
#include "wog_vobject.h"

#include <QSizeF>

struct WOGLabel
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

struct WOGSceneLayer : public WOGVObject
{
    QString id;
    QString name;
    QString image;
    QString anim;
    qreal animspeed;
};

struct WOGRadialForceField
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

struct WOGLinearForceField
{
    QString type;
    QPointF force;
    qreal dampeningfactor;
    bool antigrav;
    bool geomonly;
};

struct WOGParticle
{
    QString effect;
    qreal depth;
    QPointF position;
    qreal pretick;
};

struct WOGButton : public WOGVObject
{
    QString id;
    QString up;
    QString over;
    QString disabled;
    QString onclick;
    QString onmouseenter;
    QString onmouseexit;
};

struct WOGButtonGroup
{
    QString id;
    QPointF osx;
    QList<WOGButton*> button;

    ~WOGButtonGroup();
};

struct WOGCircle : public WOGPObject
{
    QPointF position;
    qreal radius;
};

struct WOGLine : public WOGPObject
{
    QPointF anchor;
    QPointF normal;
};

struct WOGRectangle : public WOGPObject
{
    QPointF position;
    QSizeF size;
    qreal rotation;
};

struct WOGScene
{
    qreal minx;
    qreal miny;
    qreal maxx;
    qreal maxy;
    QColor backgroundcolor;
    QList<WOGButton*> button;
    QList<WOGButtonGroup*> buttongroup;
    QList<WOGSceneLayer*> sceneLayer;
    QList<WOGLabel*> label;
    QList<WOGCircle*> circle;
    QList<WOGLine*> line;
    QList<WOGRectangle*> rectangle;
    QList<WOGLinearForceField*> linearforcefield;
    QList<WOGRadialForceField*> radialforcefield;
    QList<WOGParticle*> particle;

    ~WOGScene();
};

#endif // WOG_SCENE_H
