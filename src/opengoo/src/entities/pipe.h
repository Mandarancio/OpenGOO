#pragma once

#include <array>

#include "GameEngine/entity.h"
#include "exiteventlistener.h"

struct WOGPipe;

class Pipe : public og::Entity, public ExitEventListener
{
    enum CapType
    {
        e_open,
        e_closed
    };

public:
    Pipe(const WOGPipe& aDef, float aScale);

private:
    void Render(QPainter& aPainter);

    void OnOpen();

    void OnClosed();

private:
    std::array<GraphicPtr, 2> mCap;
    GraphicPtr mPipe;
};
