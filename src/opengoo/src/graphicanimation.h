#pragma once

#include "GameEngine/graphic.h"

#include "animation.h"

class GraphicAnimation : public Animation
{
public:
    GraphicAnimation(int aDuration)
        : Animation(aDuration)
        , mGraphic(nullptr)
    {
    }

    og::Graphic* GetGraphic() const
    {
        return mGraphic;
    }

    void SetGraphic(og::Graphic* aGraphic)
    {
        mGraphic = aGraphic;
    }

    virtual void Init(og::Graphic* aGraphic) = 0;

private:
    og::Graphic* mGraphic;
};

