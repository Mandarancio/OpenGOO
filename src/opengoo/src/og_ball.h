#ifndef OG_BALL_H
#define OG_BALL_H

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

#include <OGPhysicsBody>
#include "wog_material.h"
//#include "og_ibody.h"
//#include "wog_level.h"
//#include "wog_ball.h"

struct WOGLevelExit;
struct WOGBallInstance;
struct WOGBall;
struct WOGBallShape;

struct OGUserData;

class OGStrand;

class OGIWalkBehavior;
class OGIClimbBehavior;
class OGIFlyBehavior;

class OGWorld;

class QPainter;

class BallSensor;

class OGBall : public og::OGPhysicsBody
{
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
        bool IsSuckable() const { return isSuckable_; }
        bool IsWalking() const { return isWalking_; }

        bool IsDraggable() const { return isDraggable_; }
        bool IsDetachable() const;

        bool isExit() const { return isExit_; }
        bool isSuction() const { return isSuction_; }

        b2Vec2 GetBodyPosition() const { return body->GetPosition(); }
        QVector2D GetCenter() const;
        float getRadius() const { return shape->GetRadius(); }

        QString GetId() const;
        b2JointEdge* GetJoints() { return body->GetJointList(); }
        int GetMaxStrands() const;
        OGUserData* GetUserData();

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
        void SetOrigin(OGBall* target);
        void SetOrigin(float x, float y);
        void SetExit(bool exit);
        void SetSuction(bool suction);

        void Attache(OGBall* ball);

        void Paint(QPainter* painter, bool debug = false);
        void Update();
        void Select();
        bool TestPoint(const QPoint &pos);

        void MouseDown(const QPoint &pos);
        void MouseUp(const QPoint &pos);
        void MouseMove(const QPoint &pos);

        friend class  OGStrand;

        void touching() { _isTouching = true; }

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
        QPointF origin_;
        OGBall* pTargetBall_;
        OGBall* pOriginBall_;
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
        bool isSuckable_;
        bool isWalking_;
        bool isInit_;
        bool isSuction_;
        bool isExit_;

        QPointF* GetTarget() { return &target_; }
        QPointF* GetOrigin() { return &origin_; }
        float GetAngle() const;
        QString GetType() const;
        float GetTowerMass() const { return towerMass_; }
        WOGBallShape* GetShape() const;
        QString GetStrandType() const;
        QPointF* GetCurrentPosition() { return &curPos_; }
        void SetCurrentPosition(const b2Vec2 &pos);

        float DistanceSquared(OGBall* b1, OGBall* b2) const;
        float DistanceSquared(OGBall* b);

        float Distance(OGBall* b1, OGBall* b2) const;
        float Distance(OGBall* b);

        void SetBodyPosition(float x, float y);

        og::OGPhysicsBody* CreateCircle(float x, float y, float angle
                                    , float mass, WOGBallShape* shape
                                    , int variation);

        og::OGPhysicsBody* CreateReactangle(float x, float y, float angle
                                        , float mass, WOGBallShape* shape
                                        , int variation);

        void AddStrand();
        void ReleaseStrand();

        void Move();

        bool IsCanClimb();
        bool IsOnWalkableGeom(b2ContactEdge* edge);

        void FindJointBalls();
        void FindTarget();
        void Attache();
        void Detache();

        void Algorithm2();

        void PerformWalk();
        void PerformClimb();
        void PerformFly();

        OGIWalkBehavior* GetWalkBehavior();
        OGIClimbBehavior* GetClimbBehavior();
        OGIFlyBehavior* GetFlyBehavior();

        void SetWalkTarget(QPointF* pos);
        void SetClimbTarget(QPointF* pos);
        void SetClimbOrigin(QPointF* pos);
        void SetFlyTarget(QPointF* pos);

        void SetWalkSpeed(float speed);
        void SetClimbSpeed(float speed);

    private:
        bool _isSleeping;
        bool _isTouching;
        OGWorld* _GetWorld();
        void _RemoveStrand(OGStrand* strand);
        void _CreateStrand(OGBall* b1, OGBall* b2);

        std::unique_ptr<BallSensor> _sensor;
        std::unique_ptr<BallSensor> getSensor();
};

#endif // OG_BALL_H
