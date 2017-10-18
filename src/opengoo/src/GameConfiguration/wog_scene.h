#pragma once

#include "wog_pobject.h"
#include "wog_vobject.h"
#include "wog_circle.h"

#include <QSizeF>

struct WOGLabel
{
    QString id;
    float depth;
    QPointF position;
    QString align;
    float rotation;
    float scale;
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
    float animspeed;

    WOGSceneLayer()
        : animspeed(1.0f)
    {
    }
};

struct WOGRadialForceField
{
    QString id;
    QString type;
    QPointF center;
    float radius;
    float forceatcenter;
    float forceatedge;
    float dampeningfactor;
    bool antigrav;
    bool geomonly;
    bool enabled;
};

struct WOGLinearForceField
{
    QString type;
    QPointF force;
    float dampeningfactor;
    bool antigrav;
    bool geomonly;
};

struct WOGParticle
{
    QString effect;
    float depth;
    QPointF position;
    float pretick;
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
    QString text;
    QString font;
};

struct WOGButtonGroup
{
    QString id;
    QPointF osx;
    QList<WOGButton*> button;

    ~WOGButtonGroup();
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
    float rotation;
};

struct WOGCompositeGeom : public WOGPObject
{
    QPointF position;
    float rotation;
    QList<WOGCircle> circle;
    QList<WOGRectangle> rectangle;
};

struct WOGScene
{
    float minx;
    float miny;
    float maxx;
    float maxy;
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
    QList<WOGCompositeGeom*> compositegeom;

    ~WOGScene();

    WOGButtonGroup* GetButtonGroup(const QString& id);
    WOGButton* FindButton(const QString &id);
};
