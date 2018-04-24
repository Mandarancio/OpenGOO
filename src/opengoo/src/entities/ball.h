#pragma once

#include "GameEngine/entity.h"

class Ball : public og::Entity
{
    enum State
    {
        e_walking,
        e_unknown
    };

public:
    Ball(GraphicPtr aGraphic, float aWalkSpeed, float mClimbSpeed)
        : Entity(0, 0, aGraphic)
        , mState(e_unknown)
        , mStartWalkSpeed(aWalkSpeed)
        , mStartClimbSpeed(mClimbSpeed)
        , mWalkSpeed(0)
        , mClimbSpeed(0)
    {
    }

    void SetWalkSpeed(float aSpeed)
    {
        mWalkSpeed = aSpeed;
    }

    float GetWalkSpeed() const
    {
        return mWalkSpeed;
    }

private:
    void Update();

    bool CanWalk() const
    {
        return mWalkSpeed != 0;
    }

    void Walk()
    {
    }

    State GetState() const
    {
        return mState;
    }

private:
    State mState;
    float mStartWalkSpeed;
    float mStartClimbSpeed;
    float mWalkSpeed;
    float mClimbSpeed;
};
