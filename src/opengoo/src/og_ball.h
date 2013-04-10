#ifndef OG_BALL_H
#define OG_BALL_H

#include "og_ibody.h"
#include "wog_level.h"
#include "wog_ball.h"

#include <QPainter>
#include <QtCore/qmath.h>

// Source http://goofans.com/developers/game-file-formats/balls-xml/states-and-events
/*
    States

    These are named "states" in which the balls can exist. They describe what
        the ball is currently doing.

    attached    Attached to a structure
    climbing    Moving along a strand
    detaching   Being detached, but has not been actually removed from
        the structure
    dragging    Being held by the player
    falling     Falling or Flying.. not held by player, not sitting on geometry.
    pipe    Moving along inside the pipe
    sleeping    Asleep
    standing    On geometry, but not walking.
    stuck   Is a "sticky" ball and is stuck to geometry
    stuck_attached  Stuck to geometry and attached to a structure
    stuck_detaching     Stuck to geometry and being detached from a structure
    tank    In the tank (Final Stats screen)
    walking     On geometry and walking along.


    There is one additional state onfire which can only be used in a particles tag.
    Events

    These are named events which can happen to the balls. They describe what happened to it.
    attach  Attaches to a structure
    attachcloser    Attaches to a structure and is closer to the pipe
    bounce  Bounces when hitting geometry
    collidediff     Collides with a different type of ball
    collidegeom     Collides with a geometry object
    collidesame     collides with the same type of ball
    death   dies
    deathfall   Unknown
    detached    Is detached from a structure
    detaching   Is clicked ready for detaching
    detonate    Explodes after being on fire
    drop    Is dropped (at low speed) by the player.. not thrown
    exit    Enters the pipe
    extinguish  Unknown may be unused
    ignite  Catches fire
    land    Lands on geometry, but does not bounce.
    marker  the player moves the mouse over this ball (Hover)
    pickup  The player clicks the ball to grab it.
    snap    a strand snaps, also used for Pilot and Bit "launch" **
    suction     in range of the pipe
    throw   Released travelling at speed
*/
class OGStrand;

class OGBehavior
{
    protected:
        float x, y, speed;
        OGPhysicsBody* body;

    public:
        void SetTarget(float posx, float posy = 0) {  x = posx; y = posy; }
        void SetTarget(const QPointF* pos) { SetTarget(pos->x(), pos->y()); }
        void SetBody(OGPhysicsBody* b) { body = b; }
        void SetSpeed(float s) { speed = s; }
};

class OGIWalkBehavior : public OGBehavior
{
    public:
        virtual void Walk() = 0;
};

class OGIClimbBehavior : public OGBehavior
{
    public:
        virtual void Climb() = 0;
};

class OGIFlyBehavior : public OGBehavior
{
    public:
        virtual void Fly() = 0;
};

class OGWalk : public OGIWalkBehavior
{
    public:
        void Walk()
        {
            QVector2D* vel = body->GetVelocity();
            float dx = x - body->GetX();

            if (dx >= 0) { vel->setX(speed * 80); }
            else { vel->setX(-speed * 80); }

            body->SetVelocity(vel);
        }
};

class OGClimb : public OGIClimbBehavior
{
    public:
        void Climb()
        {
            QVector2D* vel = body->GetVelocity();
            QLineF line(*body->GetPosition(), QPointF(x, y));
            line.setLength(speed * 3);
            vel->setX(line.dx());
            vel->setY(line.dy());
            body->SetVelocity(vel);
        }
};

class OGFly : public OGIFlyBehavior
{
    public:
        void Fly() {}
};

class OGBall : public OGPhysicsBody
{
    class OGWorld* _GetWorld();
    WOGLevelExit* _GetLevelExit();
    void _RemoveStrand(OGStrand* strand);
    void _CreateStrand(OGBall* b1, OGBall* b2);

    protected:
        enum BallType {C_BALL, R_BALL}; // C_ - circle R_ - rectangle

        enum BallEvent
        {
            ATTACH
            , ATTACHCLOSER
            , MARKER
        };

        WOGBallInstance* pData_;
        WOGBall* pConfig_;
        WOGMaterial material_;
        int numberStrands_;
        BallType type_;
        int id_;
        QPointF target_;
        OGBall* pTargetBall_;
        QPointF curPos_;
        float towerMass_;
        QList<OGBall*> jointBalls_;

        OGIWalkBehavior* pWalkBehavior_;
        OGIClimbBehavior* pClimbBehavior_;
        OGIFlyBehavior* pFlyBehavior_;

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

        QPointF* GetTarget() { return &target_; }
        float GetAngle() const { return pData_->angle; }
        QString GetType() const { return pData_->type; }
        float GetTowerMass() const { return towerMass_; }
        WOGBallShape* GetShape() { return pConfig_->attribute.core.shape; }
        QString GetStrandType() const { return pConfig_->stand->type; }
        QPointF* GetCurrentPosition() { return &curPos_; }
        void SetCurrentPosition(const b2Vec2 &pos);

        float DistanceSquared(OGBall* b1, OGBall* b2);
        float DistanceSquared(OGBall* b) { return DistanceSquared(b, this); }

        float Distance(OGBall* b1, OGBall* b2);
        float Distance(OGBall* b) { return qSqrt(DistanceSquared(b)); }

        void SetBodyPosition(float x, float y);

        OGPhysicsBody* CreateCircle(float x, float y, float angle
                                    , float mass, WOGBallShape* shape
                                    , int variation);

        OGPhysicsBody* CreateReactangle(float x, float y, float angle
                                        , float mass, WOGBallShape* shape
                                        , int variation);

        void AddStrand();
        void ReleaseStrand();

        void Move();

        bool IsCanClimb();
        bool IsOnWalkableGeom(b2ContactEdge* edge);
        bool IsTached(float x, float y);

        void FindJointBalls();
        void FindTarget();
        void Attache();
        void Detache();

        void Algorithm2();

        void PerformWalk() { pWalkBehavior_->Walk(); }
        void PerformClimb() { pClimbBehavior_->Climb(); }
        void PerformFly() { pFlyBehavior_->Fly(); }

        void SetWalkTarget(QPointF* pos) { pWalkBehavior_->SetTarget(pos->x()); }
        void SetClimbTarget(QPointF* pos) { pClimbBehavior_->SetTarget(pos); }
        void SetFlyTarget(QPointF* pos) { pFlyBehavior_->SetTarget(pos); }

        void SetBodyWalk() { pWalkBehavior_->SetBody(this); }
        void SetBodyClimb() { pClimbBehavior_->SetBody(this); }
        void SetBodyFly() { pFlyBehavior_->SetBody(this); }

        void SetWalkSpeed(float speed) { pWalkBehavior_->SetSpeed(speed); }
        void SetClimbSpeed(float speed) { pClimbBehavior_->SetSpeed(speed); }

    public:
        OGBall(WOGBallInstance* data, WOGBall* configuration);
        virtual ~OGBall();

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
        bool IsDetachable() const { return pConfig_->attribute.player.detachable; }

        b2Vec2 GetBodyPosition() const { return body->GetPosition(); }
        QString GetId() const { return pData_->id; }
        b2JointEdge* GetJoints() { return body->GetJointList(); }
        int GetMaxStrands() const { return pConfig_->attribute.core.strands; }

        // Set properties
        void SetAttached(bool status) { isAttached_ = status; }
        void SetClimbing(bool status) { isClimbing_ = status; }
        void SetDetaching(bool status) { isDetaching_ = status; }
        void SetDragging(bool status) { isDragging_ = status; }
        void SetFalling(bool status) { isFalling_ = status; }
        void SetMarked(bool status);
        void SetStanding(bool status) { isStanding_ = status; }
        void SetWalking(bool status) { isWalking_ = status; }

        void SetId(int id) { id_ = id; }
        void SetTarget(OGBall* target);
        void SetTarget(float x, float y);
        void Attache(OGBall* ball);

        void Paint(QPainter* painter, bool debug = false);
        void Update();
        void Select();
        bool TestPoint(const QPoint &pos);

        void MouseDown(const QPoint &pos);
        void MouseUp(const QPoint &pos);
        void MouseMove(const QPoint &pos);

        friend class  OGStrand;
};

#endif // OG_BALL_H
