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
};

class Ball;

struct PathFinder
{
    virtual ~PathFinder()
    {
    }

    virtual void Init(Ball* aStart) = 0;

    virtual Ball* Next() = 0;
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
    Ball(std::unique_ptr<og::PhysicsBody> aBody, GraphicPtr aGraphic, const BallDefination& aDef);

    float GetStartWalkSpeed() const
    {
        return mStartWalkSpeed;
    }

    void SetWalkSpeed(float aSpeed)
    {
        mWalkSpeed = aSpeed;
        GetPhysicsBody()->SetAngularVelocity(mWalkSpeed);
    }

    float GetWalkSpeed() const
    {
        return mWalkSpeed;
    }

    bool IsWalking() const
    {
        return mWalkSpeed != 0;
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
        return mClimbSpeed != 0;
    }

    void SetListener(BallListener* aListener)
    {
        mListener = aListener;
    }

    void StartWalk()
    {
        if (IsWalking() || IsAttached() || IsSleeping() || mIsMarked)
        {
            return;
        }

        SetWalkSpeed(mStartWalkSpeed);
    }

    void StopWalk()
    {
        SetWalkSpeed(0);
    }

    void StartClimb(Ball *aBall);

    void UnMark()
    {
        mIsMarked = false;
        mMarkerGraphic.reset();
    }

    void Mark(std::shared_ptr<og::Graphic> aGraphic)
    {
        mIsMarked = true;
        StopWalk();
        mClimbSpeed = 0;
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

    const og::physics::Shape* GetShape() const
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
        GetPhysicsBody()->SetActive(true);
        SetWalkSpeed(0);
    }

private:
    void Update();

    void Render(QPainter &aPainter);

    void Debug(QPainter& aPainter);

    void Reverse();

    void Collide(og::Entity* aEntity, const og::physics::Contact& aContact);

    void Collide(Ball* aEntity, const og::physics::Contact&);

    void Collide(Geom* aEntity, const og::physics::Contact& aContact);

    void LastUpdate();

    void BuildPath(Ball* aBall, bool aCheckNearest);

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
};
