#pragma once

#include "animationgroup.h"

namespace og
{
class Entity;
}

class Animator
{
public:
    void Update();

    std::shared_ptr<AnimationGroup> AddAnimationGroup(std::shared_ptr<AnimationGroup> aGroup)
    {
        mAnimationGroup.push_back(aGroup);
        return aGroup;
    }

private:
    std::vector<std::shared_ptr<AnimationGroup>> mAnimationGroup;
};

