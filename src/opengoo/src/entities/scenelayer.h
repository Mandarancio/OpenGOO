#pragma once

#include "GameEngine/entity.h"
#include "animator.h"

class SceneLayer : public og::Entity
{
    std::shared_ptr<Animator> mAnimator;

public:
    SceneLayer(float aX, float aY, GraphicPtr aGraphic)
        : Entity(aX, aY, aGraphic)
    {
    }

    void SetAnimator(std::shared_ptr<Animator> aAnimator)
    {
        mAnimator = aAnimator;
    }

    void Update()
    {
        if (mAnimator)
        {
            mAnimator->Update();
        }
    }
};
