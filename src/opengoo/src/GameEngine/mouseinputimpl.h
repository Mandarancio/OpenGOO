#pragma once

#include "input.h"

namespace og
{
class MouseInputImpl
{
public:
    static bool IsButtonPressed(MouseInput::MouseButton aButton);

    static QPoint GetPosition();

    static QPoint GetWorldPosition();
};
}
