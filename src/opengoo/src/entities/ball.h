#pragma once

#include "GameEngine/entity.h"

class Ball : public og::Entity
{
public:
    Ball(GraphicPtr a_graphic)
        : Entity(0, 0, a_graphic)
    {
    }

private:
    void Update();
};
