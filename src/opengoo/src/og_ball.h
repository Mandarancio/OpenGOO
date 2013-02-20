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
class OGStrand;

class OGBall : public OGPhysicsBody
{
protected:
    enum BallType {C_BALL, R_BALL}; // C_ - circle R_ - rectangle

    enum BallEvent
    {
        ATTACH
        , ATTACHCLOSER
        , MARKER
    };

    WOGBallInstance* data_;
    WOGBall* config_;
    WOGMaterial* material_;
    int numberStrands_;
    BallType type_;
    int id_;
    QPointF* target_;
    OGBall* targetBall_;
    QPointF* curPos_;
    float towerMass_;
    QList<OGBall*> jointBalls_;

    bool isAttached_;
    bool isClimbing_;
    bool isDetaching_;
    bool isDraggable_;
    bool isDragging_;
    bool isFalling_;
    bool isMarked_;
    bool isStanding_;
    bool isWalking_;
    bool isInit_;

    QPointF* GetTarget() const;
    float GetAngle() const { return data_->angle; }
    QString GetType() const { return data_->type; }
    float GetTowerMass() const { return towerMass_; }
    WOGBallShape* GetShape() { return config_->attribute.core.shape; }
    QString GetStrandType() const { return config_->stand->type; }
    QPointF* GetCurrentPosition()  const { return curPos_; }

    void SetCurrentPosition(const b2Vec2 & pos);
    void SetBodyPosition(float x, float y);

    OGPhysicsBody* CreateCircle(float x, float y, float angle
                                 , float mass, WOGBallShape* shape
                                 , int variation);

    OGPhysicsBody* CreateReactangle(float x, float y, float angle
                                     , float mass, WOGBallShape* shape
                                     , int variation);

    void AddStrand();
    void ReleaseStrand();

    void Climbing(float x, float y);
    void Move();
    void Walk(float x);    
    void Walk(const QPointF & pos);

    bool IsCanClimb();        
    bool IsOnWalkableGeom(b2ContactEdge* edge);
    bool IsTached(float x, float y);

    void FindJointBalls();
    void FindTarget();
    void Attache();
    void Detache();

public:
    OGBall(WOGBallInstance* data, WOGBall* configuration);
    virtual ~OGBall() { delete target_; }

    // Get properties
    int id() const { return id_; }

    bool IsAttached() const { return isAttached_; }
    bool IsClimbing() const { return isClimbing_; }
    bool IsDragging() const { return isDragging_; }
    bool IsFalling() const { return isFalling_; }
    bool IsMarked() const { return isMarked_; }
    bool IsStanding() const { return isStanding_; }
    bool IsWalking() const { return isWalking_; }

    bool IsDraggable() const { return isDraggable_; }
    bool IsDetachable() const { return config_->attribute.player.detachable; }

    b2Vec2 GetBodyPosition() const { return body->GetPosition(); }
    QString GetId() const { return data_->id; }
    int GetMaxStrands() const { return config_->attribute.core.strands; }
    float GetX() const { return body->GetPosition().x; }
    float GetY() const { return body->GetPosition().y; }

    // Set properties
    void SetAttached(bool status) { isAttached_ = status; }
    void SetClimbing(bool status) { isClimbing_ = status; }
    void SetDetaching(bool status) { isDetaching_ = status; }
    void SetDragging(bool status) { isDragging_ = status; }
    void SetFalling(bool status) { isFalling_ = status; }
    void SetMarked(bool status) { isMarked_ = status; Select(); }
    void SetStanding(bool status) { isStanding_ = status; }
    void SetWalking(bool status) { isWalking_ = status; }

    void SetId(int id) { id_ = id; }
    void SetTarget(OGBall* target) { targetBall_ = target; }

    void Attache(OGBall* ball);

    void Paint(QPainter* painter, bool debug=false);
    void Update();
    void Select();
    bool TestPoint(const QPoint & pos);

    void MouseDown(const QPoint & pos);
    void MouseUp(const QPoint & pos);
    void MouseMove(const QPoint & pos);

    friend class  OGStrand;
};

#endif // OG_BALL_H
