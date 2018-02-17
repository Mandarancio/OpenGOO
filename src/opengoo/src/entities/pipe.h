#pragma once

#include <array>

#include "GameEngine/entity.h"

class MultiSprite;

class Pipe : public og::Entity
{
public:
    Pipe(const WOGPipe& aDef);

private:
    void Render(QPainter& aPainter);

private:
    std::array<GraphicPtr, 2> mCap;
    GraphicPtr mPipe;
};
