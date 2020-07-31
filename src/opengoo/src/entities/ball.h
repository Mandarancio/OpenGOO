#pragma once

#include <queue>

#include "GameEngine/entity.h"
#include "GameEngine/Colliders/collider.h"

#include "animator.h"

class Geom;
class Strand;

struct BallListener;

class PhysicsCollider : public og::Collider
{
    bool OverlapPoint(const QVector2D& a_point) const;
};

struct BallDefination
{
    float walkSpeed;
    float climbSpeed;
    bool isSleeping;
    bool isSuckable;
    bool isDraggable;
    float mass;
};

class Ball;

struct PathFinder
{
    virtual ~PathFinder()
    {
    }

    virtual void Init(Ball* aStart) = 0;

    virtual Ball* Next() = 0;

    virtual Ball* Start() = 0;
};

struct BallPathFinder : public PathFinder
{
    BallPathFinder()
        : ShoulCheckNearest(true)
        , Depth(0)
    {
    }

    bool ShoulCheckNearest;
    size_t Depth;
};

class Ball : public og::Entity
{
public:
    Ball(const QString& aType, std::unique_ptr<og::PhysicsBody> aBody, GraphicPtr aGraphic, const BallDefination& aDef);

    bool IsWalking() const
    {
        return GetPhysicsBody()->GetAngularVelocity() != 0;
    }

    void SetAttached(bool aFlag)
    {
        mIsAttached = aFlag;
    }

    bool IsAttached() const
    {
        return mIsAttached;
    }

    bool IsMarker() const
    {
        return mIsMarked;
    }

    bool IsDragging() const
    {
        return mIsDragging;
    }

    bool IsDraggable() const
    {
        return mIsDraggable;
    }

    bool IsStanding() const
    {
        return mIsStanding;
    }

    bool IsSleeping() const
    {
        return mIsSleeping;
    }

    bool IsClimbing() const
    {
        return mIsClimbing;
    }

    bool IsSuckable() const
    {
        return mIsSuckable;
    }

    bool IsSucked() const
    {
        return mIsSucked;
    }

    void SetListener(BallListener* aListener)
    {
        mListener = aListener;
    }

    float GetStartWalkSpeed() const
    {
        return mStartWalkSpeed;
    }

    float GetStartClimbSpeed() const
    {
        return mStartClimbSpeed;
    }

    float GetClimbSpeed() const
    {
        return mClimbSpeed;
    }

    float GetWalkSpeed() const
    {
        return mWalkSpeed;
    }

    void StartWalk();

    void StopWalk()
    {
        if (!IsWalking())
        {
            return;
        }

        SetAngularVelocity(0);
    }

    void SetClimb(bool aClimb)
    {
        mIsClimbing = aClimb;
    }

    void StartClimb(Ball *aBall);

    void StopClimb()
    {
        SetClimb(false);
    }

    void UnMark()
    {
        mIsMarked = false;
        if (GetWalkSpeed() != 0)
        {
            StartWalk();
        }
        else if (GetClimbSpeed() != 0)
        {
            SetClimb(true);
        }
        mMarkerGraphic.reset();
    }

    void Mark(std::shared_ptr<og::Graphic> aGraphic)
    {
        StopWalk();
        StopClimb();
        mIsMarked = true;
        mMarkerGraphic = aGraphic;
    }

    void OnPickUp();

    void OnAttach();

    void Drop();

    void SetAnimator(std::shared_ptr<Animator> aAnimator)
    {
        mAnimator = aAnimator;
    }

    void AddStrand(Strand* aStrand)
    {
        mStrand.push_back(aStrand);
    }

    void RemoveStrand(Strand* aStrand);

    Strand* GetStrand(size_t aIndex) const
    {
        return mStrand[aIndex];
    }

    size_t StrandCount() const
    {
        return mStrand.size();
    }

    QVector2D GetPhyPosition() const
    {
        return GetPhysicsBody()->GetPosition();
    }

    void SetPhyPosition(const QVector2D& aPosition)
    {
        GetPhysicsBody()->SetPosition(aPosition);
    }

    og::physics::Shape GetShape() const
    {
        return GetPhysicsBody()->GetShape();
    }

    bool IsNearest() const
    {
        return mIsNearest;
    }

    void SetIsNearest(bool aIsNearest)
    {
        mIsNearest = aIsNearest;
    }

    void OnSuction()
    {
        StopClimb();
    }

    void OnExit();

    bool IsActive() const
    {
        return GetPhysicsBody()->IsActive();
    }

    void SetActive(bool aActive)
    {
        GetPhysicsBody()->SetActive(aActive);
    }

    void PhyMoveBy(const QVector2D& aOffset)
    {
        SetPhyPosition(GetPhyPosition() + aOffset);
    }

    bool GetWalkDirection() const
    {
        return mWalkDirection;
    }

private:
    void Update();

    void Render(QPainter &aPainter);

    void Debug(QPainter& aPainter);

    void ReverseWalk();

    void Collide(og::Entity* aEntity, const og::physics::Contact& aContact);

    void Collide(Ball* aEntity, const og::physics::Contact&);

    void Collide(Geom* aEntity, const og::physics::Contact& aContact);

    void LastUpdate();

    void BuildPath(Ball* aBall, bool aCheckNearest);

    void SetClimbSpeed(float aSpeed)
    {
        mClimbSpeed = aSpeed;
    }

    void SetWalkSpeed(float aSpeed)
    {
        mWalkSpeed = aSpeed;
    }

    void SetAngularVelocity(float aSpeed);

    void SetWalkDirection(bool aDirection)
    {
        if (mWalkDirection == aDirection)
        {
            return;
        }

        mWalkDirection = aDirection;
        if (GetWalkSpeed() != 0)
        {
            auto speed = mWalkDirection ? GetWalkSpeed() : -GetWalkSpeed();
            SetAngularVelocity(speed);
        }
    }

    QVector2D NextClimbPosition();

private:
    float mStartWalkSpeed;
    float mStartClimbSpeed;
    float mWalkSpeed;
    float mClimbSpeed;
    bool mIsSleeping;
    bool mIsAttached;
    bool mIsDragging;
    bool mIsMarked;
    bool mIsStanding;
    bool mIsSuckable;
    bool mIsClimbing;
    bool mIsDraggable;
    bool mIsSucked;

    QVector2D mPrevPosition;
    BallListener* mListener;
    std::shared_ptr<Animator> mAnimator;
    std::shared_ptr<og::Graphic> mMarkerGraphic;
    og::PhysicsBody* mCollideWith;
    std::vector<Strand*> mStrand;
    Ball* mStartBall;
    Ball* mEndBall;
    bool mIsNearest;
    std::queue<Ball*> mPath;
    std::unique_ptr<BallPathFinder> mPathFinder;
    int mDelay;
    bool mWalkDirection;
    float mPassedPath;
};
