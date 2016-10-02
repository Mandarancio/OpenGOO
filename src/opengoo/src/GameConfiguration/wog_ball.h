#ifndef WOG_BALL_H
#define WOG_BALL_H

// Ball definition file
// source http://goofans.com/developers/game-file-formats/balls-xml

#include <QString>
#include <QColor>

struct WOGBallShape
{
    enum Type
    {
        e_unknown,
        e_circle,
        e_rectangle
    };

    float variation;

    WOGBallShape(float variation)
        : variation(variation)
    {
        type = e_unknown;
    }

    Type type;
};

struct WOGCircleBall : public WOGBallShape
{
    float radius;

    WOGCircleBall(float diameter=0, float variation=0)
        : WOGBallShape(variation)
    {
        type = e_circle;
        radius = diameter*0.5f;
    }
};

struct WOGRectangleBall : public WOGBallShape
{
    float width;
    float height;

    WOGRectangleBall(float width=0, float height=0, float variation=0)
        : WOGBallShape(variation), width(width), height(height)
    {
        type = e_rectangle;
    }
};

struct WOGBallCoreAttributes
{
    QString name;
    std::shared_ptr<WOGBallShape> shape;
    float mass;
    int strands;
    QString material;
    float towermass;
    float dragmass;
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
    float walkforce;
};

struct WOGBallPlayerInteraction
{
    bool detachable;
    bool draggable;
    bool hingedrag;
    QString fling;
};

struct WOGBallLevelInteraction
{
    bool suckable;
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

class OGBallConfig;

struct WOGBall
{
    typedef OGBallConfig Conf;

    WOGBallAttributes attribute;
    WOGBallStrand* strand;
    WOGBallDetachstrand* detachstrand;

    WOGBall() : strand(0), detachstrand(0)  {}
    ~WOGBall()
    {
        if (strand) { delete strand; }

        if (detachstrand) { delete detachstrand; }
    }
};

#endif // WOG_BALL_H
