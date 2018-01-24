#pragma once

#include <vector>
#include <memory>

#include "animation.h"

class AnimationGroup
{
public:
    virtual ~AnimationGroup()
    {
    }

    void AddAnimation(std::shared_ptr<Animation> aAnimation)
    {
        mAnimation.push_back(aAnimation);
    }

    virtual void Update(int aDt) = 0;

    virtual void Start() = 0;

protected:
    std::vector<std::shared_ptr<Animation>> mAnimation;
};
