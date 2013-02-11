#ifndef OG_BALL_H
#define OG_BALL_H

#include "og_ibody.h"
#include "wog_level.h"
#include "wog_ball.h"

#include <QPainter>

// Source http://goofans.com/developers/game-file-formats/balls-xml/states-and-events
/*
    States

    These are named "states" in which the balls can exist. They describe what
        the ball is currently doing.

    attached 	Attached to a structure
    climbing 	Moving along a strand
    detaching 	Being detached, but has not been actually removed from
        the structure
    dragging 	Being held by the player
    falling 	Falling or Flying.. not held by player, not sitting on geometry.
    pipe 	Moving along inside the pipe
    sleeping 	Asleep
    standing 	On geometry, but not walking.
    stuck 	Is a "sticky" ball and is stuck to geometry
    stuck_attached 	Stuck to geometry and attached to a structure
    stuck_detaching 	Stuck to geometry and being detached from a structure
    tank 	In the tank (Final Stats screen)
    walking 	On geometry and walking along.


    There is one additional state onfire which can only be used in a particles tag.
    Events

    These are named events which can happen to the balls. They describe what happened to it.
    attach 	Attaches to a structure
    attachcloser 	Attaches to a structure and is closer to the pipe
    bounce 	Bounces when hitting geometry
    collidediff 	Collides with a different type of ball
    collidegeom 	Collides with a geometry object
    collidesame 	collides with the same type of ball
    death 	dies
    deathfall 	Unknown
    detached 	Is detached from a structure
    detaching 	Is clicked ready for detaching
    detonate 	Explodes after being on fire
    drop 	Is dropped (at low speed) by the player.. not thrown
    exit 	Enters the pipe
    extinguish 	Unknown may be unused
    ignite 	Catches fire
    land 	Lands on geometry, but does not bounce.
    marker 	the player moves the mouse over this ball (Hover)
    pickup 	The player clicks the ball to grab it.
    snap 	a strand snaps, also used for Pilot and Bit "launch" **
    suction 	in range of the pipe
    throw 	Released travelling at speed
*/

struct OGBall : public OGPhysicsBody
{
    enum BallType {C_BALL, R_BALL}; // C_ - circle R_ - rectangle

    WOGBallInstance* m_data;
    WOGBall* m_configuration;
    WOGMaterial* m_material;
    float m_K;
    int m_numberStrands;
    BallType m_type;

    bool attached;
    bool detaching;
    bool dragging;
    bool standing;
    bool walking;

    OGBall(WOGBallInstance* data, WOGBall* configuration);
    virtual ~OGBall() {}

    float32 GetAngle() const { return m_data->angle; }
    float32 GetX() const { return body->GetPosition().x; }
    float32 GetY() const { return body->GetPosition().y; }
    QString GetType() const { return m_data->type; }
    QString GetId() const { return m_data->id; }
    b2Vec2 GetBodyPosition() const { return body->GetPosition(); }
    float GetMass() const { return m_configuration->attribute.core.mass; }
    int GetMaxStrands() const
    {
        return m_configuration->attribute.core.strands;
    }

    WOGBallShape* GetShape() { return m_configuration->attribute.core.shape; }
    QString GetStrandType() const { return m_configuration->stand->type; }
    bool IsDraggable() const
    {
        return m_configuration->attribute.player.draggable;
    }

    bool IsDetachable() const
    {
        return m_configuration->attribute.player.detachable;
    }

    bool IsDynamic() const { return true; }

    void SetBodyPosition(float32 x, float32 y);

    OGPhysicsBody* m_CreateCircle(float32 x, float32 y, float32 angle
                                 , float mass, WOGBallShape* shape
                                 , int variation);

    OGPhysicsBody* m_CreateReactangle(float32 x, float32 y, float32 angle
                                     , float mass, WOGBallShape* shape
                                     , int variation);

    void m_AddStrand() { m_numberStrands++; attached = true; }
    void m_ReleaseStrand()
    {
        m_numberStrands--;

        if (m_numberStrands == 0) { attached = false; }
    }

    void Attache(OGBall* ball);
    void Detache();
    void Move(const QPointF &pos);
    bool Select(const QPoint & pos);
    void Walk(const QPointF & pos);
    bool testWalkable();
    void Event(const QString & event);
    void Paint(QPainter* painter, bool debug=false);
};

#endif // OG_BALL_H
