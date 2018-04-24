#pragma once

#include <QSizeF>

#include "OGLib/optional.h"

#include "wog_circle.h"
#include "wog_vobject.h"

struct WOGLabel : public WOGVObject
{
    QString align;
    bool overlay;
    bool screenspace;
    QString font;
    QString text;
};

struct WOGSceneLayer : public WOGVObject
{
    QString name;
    QString image;
    QString anim;
    float animspeed;

    WOGSceneLayer()
        : animspeed(1)
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

    WOGRadialForceField()
        : radius(0)
        , forceatcenter(0)
        , forceatedge(0)
        , dampeningfactor(0)
    {
    }
};

struct WOGLinearForceField
{
    QString id;
    QString type; // gravity, force
    QPointF force;
    float dampeningfactor;
    bool antigrav;
    bool geomonly;
    float width;
    float height;
    oglib::Optional<QPointF> center;
    oglib::Optional<QColor> color;
    oglib::Optional<float> depth;
    oglib::Optional<bool> enabled;
    oglib::Optional<bool> water;

    WOGLinearForceField()
        : dampeningfactor(0)
        , antigrav(false)
        , geomonly(false)
        , width(0)
        , height(0)
    {
    }
};

struct WOGButton : public WOGVObject
{
    QString up;
    QString over;
    QString disabled;
    QString onclick;
    QString onmouseenter;
    QString onmouseexit;
    QString tooltip;
};

struct WOGButtonGroup
{
    QString id;
    QPointF osx;
    std::list<WOGButton> button;
};

struct WOGLine : public WOGPhysicsObject
{
    QPointF normal;
};

struct WOGRectangle : public WOGPhysicsObject
{
    float width;
    float height;
    WOGImage image;

    WOGRectangle()
        : width(0)
        , height(0)
    {
    }

    bool HasImage() const
    {
        return !image.IsEmpty();
    }
};

struct WOGCompositeGeom : public WOGPhysicsObject
{
    std::list<WOGCircle> circle;
    std::list<WOGRectangle> rectangle;
};

struct WOGHinge
{
    QString body1;
    QString body2;
    QPointF anchor;
    oglib::Optional<float> lostop;
    oglib::Optional<float> histop;
    oglib::Optional<float> bounce;
};

struct WOGMotor
{
    QString body;
    float speed;
    float maxforce;
};

struct WOGScene
{
    struct WOGParticle
    {
        QString effect;
        float depth;
        QPointF position;
        float pretick;

        WOGParticle()
            : depth(0)
            , pretick(0)
        {
        }
    };

    float minx;
    float miny;
    float maxx;
    float maxy;
    QColor backgroundcolor;
    std::list<WOGButton> button;
    std::list<WOGButtonGroup> buttongroup;
    std::list<WOGSceneLayer> sceneLayer;
    std::list<WOGLabel> label;
    std::list<WOGCircle> circle;
    std::list<WOGLine> line;
    std::list<WOGRectangle> rectangle;
    std::list<WOGLinearForceField> linearforcefield;
    std::list<WOGRadialForceField> radialforcefield;
    std::list<WOGParticle> particle;
    std::list<WOGCompositeGeom> compositegeom;
    std::list<WOGHinge> hinge;
    std::list<WOGMotor> motor;

    WOGScene()
        : minx(0)
        , miny(0)
        , maxx(0)
        , maxy(0)
    {
    }

    static std::unique_ptr<WOGScene> Create()
    {
        return std::unique_ptr<WOGScene>(new WOGScene);
    }
};
