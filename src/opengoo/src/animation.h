#pragma once

#include "GameEngine/entity.h"

class Animation
{
public:
    Animation(int aDuration)
        : mDuration(aDuration)
    {
    }

    virtual ~Animation()
    {
    }

    bool IsActive() const
    {
        return mIsActive;
    }

    virtual void Restart() = 0;

    virtual void Start() = 0;

    virtual void Update(int aDt) = 0;

protected:
    int mDuration;
    int mRemainingTime;
    bool mIsActive;
};
