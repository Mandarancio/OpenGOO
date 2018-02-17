#pragma once

#include <vector>
#include <memory>

#include "animation.h"

class AnimationGroup
{
public:
    AnimationGroup()
        : mLoop(false)
        , mIsActive(false)
    {
    }

    virtual ~AnimationGroup()
    {
    }

    void AddAnimation(std::shared_ptr<Animation> aAnimation)
    {
        mAnimation.push_back(aAnimation);
    }

    virtual void Update(int aDt) = 0;

    virtual void Start(bool aLoop) = 0;

    void SetLoop(bool aLoop)
    {
        mLoop = aLoop;
    }

    bool GetLoop() const
    {
        return mLoop;
    }

    bool IsActive() const
    {
        return mIsActive;
    }

protected:
    std::vector<std::shared_ptr<Animation>> mAnimation;
    bool mLoop;
    bool mIsActive;
};
