#ifndef WOG_BALL_H
#define WOG_BALL_H

#include <QDebug>

// Ball definition file
// source http://goofans.com/developers/game-file-formats/balls-xml

struct WOGBallCoreAttributes
{
    QString name;
    QString shape;
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

struct WOGBall
{
    WOGBallAttributes attribute;
    ~WOGBall()
    {
        qDebug("WOGBall: End");
    }
};

#endif // WOG_BALL_H
