#include "input.h"

#include "mouseinputimpl.h"

namespace og
{
QPoint MouseInput::GetPosition()
{
    return MouseInputImpl::GetPosition();
}

QPoint MouseInput::GetWorldPosition()
{
    return MouseInputImpl::GetWorldPosition();
}

bool MouseInput::IsButtonPressed(MouseButton aButton)
{
    return MouseInputImpl::IsButtonPressed(aButton);
}
}
