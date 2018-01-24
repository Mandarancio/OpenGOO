#pragma once

#include "GameEngine/entity.h"

class Animation
{
public:
    Animation(og::Entity* aEntity, int aDuration)
        : mEntity(aEntity)
        , mDuration(aDuration)
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
    og::Entity* mEntity;
    int mDuration;
    int mRemainingTime;
    bool mIsActive;
};
