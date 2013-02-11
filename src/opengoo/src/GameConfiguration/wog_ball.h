#ifndef WOG_BALL_H
#define WOG_BALL_H

#include <QDebug>

// Ball definition file
// source http://goofans.com/developers/game-file-formats/balls-xml

struct WOGBallShape
{
    QString type;
    float variation;

    WOGBallShape(QString shapetype, float variation=0)
        : type(shapetype), variation(variation) {}
};

struct WOGCircleBall : public WOGBallShape
{
    float radius;

    WOGCircleBall(float diameter=0, float variation=0)
        : WOGBallShape("circle", variation)
    {
        radius = diameter*0.5;
    }
};

struct WOGRectangleBall : public WOGBallShape
{
    float width;
    float height;

    WOGRectangleBall(float width=0, float height=0, float variation=0)
        : WOGBallShape("rectangle", variation), width(width), height(height){}
};

struct WOGBallCoreAttributes
{
    QString name;
    WOGBallShape* shape;
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

struct WOGBall
{
    WOGBallAttributes attribute;
    WOGBallStrand* stand;
    WOGBallDetachstrand* detachstrand;

    WOGBall() : stand(0), detachstrand(0)  {}
    ~WOGBall()
    {
        if (stand) { delete stand; }

        if (detachstrand) { delete detachstrand; }

        qDebug("WOGBall: End");
    }
};

#endif // WOG_BALL_H
