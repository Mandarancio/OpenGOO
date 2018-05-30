#include "mouseinputimpl.h"

#include "GameEngine/og_gameengine.h"

namespace og
{
bool MouseInputImpl::IsButtonPressed(MouseInput::MouseButton aButton)
{
    return GE->getWindow()->IsButtonPressed(aButton);
}

QPoint MouseInputImpl::GetPosition()
{
    return GE->getWindow()->GetMousePosition();
}

QPoint MouseInputImpl::GetWorldPosition()
{
    return GE->windowToLogical(GetPosition());
}
}
