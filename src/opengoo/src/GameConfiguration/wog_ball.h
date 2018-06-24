#pragma once
// Ball definition file
// source http://goofans.com/developers/game-file-formats/balls-xml

#include <QStringList>
#include <QColor>
#include <QVector2D>
#include <QMap>

#include "OGLib/optional.h"

struct WOGBallShape
{
    enum Type
    {
        e_unknown,
        e_circle,
        e_rectangle
    };

    Type type;
    float variation;
    float radius;
    float width;
    float height;

    WOGBallShape()
    {
        type = e_unknown;
    }
};

struct WOGBallCoreAttributes
{
    QString name;
    WOGBallShape shape;
    float mass;
    int strands;
    QString material;
    float towermass;
    oglib::Optional<float> dragmass;
};

struct WOGBallBehaviourAttributes
{
    QString jump;
};

struct WOGBallMovementAttributes
{
    float walkspeed;
    float climbspeed;
    float speedvariance;
    oglib::Optional<float> dampening;
    oglib::Optional<float> walkforce;
    oglib::Optional<float> thrust;

    WOGBallMovementAttributes()
        : walkspeed(0)
        , climbspeed(0)
        , speedvariance(0)
    {
    }
};

struct WOGBallPlayerInteraction
{
    bool detachable;
    bool draggable;
    bool hingedrag;
    QString fling;

    WOGBallPlayerInteraction()
        : draggable(true)
    {
    }
};

struct WOGBallLevelInteraction
{
    bool suckable;

    WOGBallLevelInteraction()
        : suckable(true)
    {
    }
};

// Other Ball Interaction

struct WOGBallOtherInteraction
{
};

struct WOGBallCosmeticAttributes
{
    QColor blinkcolor;
};

struct WOGBallBurnAttributes
{
};

// Popping / MOM Attributes

struct WOGBallPoppingMOMAttributes
{
};

struct WOGBallAttributes
{
    WOGBallCoreAttributes core;
    WOGBallBehaviourAttributes behaviour;
    WOGBallMovementAttributes movement;
    WOGBallPlayerInteraction player;
    WOGBallLevelInteraction level;
    WOGBallOtherInteraction other;
    WOGBallCosmeticAttributes cosmetic;
    WOGBallBurnAttributes burn;
    WOGBallPoppingMOMAttributes poppingmom;

    QString spawn; // Dispenser Attributes
};

// Source http://goofans.com/developers/game-file-formats/strands

struct WOGBallStrand
{
    QString type;
    QString image;
    QString inactiveimage;
    float minlen;
    float maxlen2;
    float maxlen1;
    float shrinklen;
    float thickness;
    float springconstmax;
    float springconstmin;
    bool walkable;
    float dampfac;
    float maxforce;
    float  burnspeed;
    float ignitedelay;
    QString burntimage;
    QString fireparticles;
    bool rope;
    bool geom;
};

struct WOGBallDetachstrand
{
    QString image;
    float maxlen;
};

struct WOGPart
{
    QString name;
    QStringList image;
    std::vector<float> x;
    std::vector<float> y;
    int layer;
    QStringList state;
    float scale;
    bool rotate;
    QString stretch; // {speed},{direction scale},{across scale} : Default = No Stretching
    bool eye;
    QString pupil;
    int pupilinset;
    std::vector<float> xrange;
    std::vector<float> yrange;

    WOGPart()
        : layer(0)
        , scale(1.0f)
        , rotate(false)
        , eye(false)
        , pupilinset(0)
    {
    }
};

struct WOGBallMarker
{
    QString drag;
    QString detach;
    float rotspeed;

    WOGBallMarker()
        : rotspeed(0)
    {
    }
};

struct WOGBall
{
    WOGBallAttributes attribute;
    std::unique_ptr<WOGBallStrand> strand;
    std::unique_ptr<WOGBallDetachstrand> detachstrand;
    QList<WOGPart> parts;
    QMap<QString, QStringList> sounds;
    WOGBallMarker marker;

    static std::unique_ptr<WOGBall> Create()
    {
        return std::unique_ptr<WOGBall>(new WOGBall);
    }
};
