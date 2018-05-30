#pragma once

#include <QPoint>

namespace og
{
class MouseInput
{
public:
    enum MouseButton
    {
        e_left,
        e_middle,
        e_right,
        e_none,
        e_any
    };

public:
    static QPoint GetPosition();

    static QPoint GetWorldPosition();

    static bool IsButtonPressed(MouseButton aButton);
};
}
