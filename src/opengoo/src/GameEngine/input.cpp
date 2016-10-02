#include "GameEngine/og_gameengine.h"

#include "input.h"

namespace og
{
QPoint MouseInput::GetPosition()
{
    auto w = GE->getWindow();
    auto pos = w->cursor().pos();
    return w->mapFromGlobal(pos);
}
}
