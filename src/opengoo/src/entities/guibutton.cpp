#include "GameEngine/input.h"

#include "guibutton.h"

GUIButton::GUIButton(std::shared_ptr<og::Collider> aCollider, float aX, float aY, GraphicPtr aUp, GraphicPtr aOver,
                     const Callback& aCallback)
    : Button(aX, -aY, aUp, aOver, aCallback)
{
    SetDepth(1000);
    SetCollider(std::move(aCollider));
}

void GUIButton::Render(QPainter& aPainer)
{
    aPainer.save();
    aPainer.resetTransform();
    GetGraphic()->Render(aPainer, GetX(), -GetY());
    aPainer.restore();
}

QVector2D GUIButton::GetMousePosition() const
{
    return QVector2D(og::MouseInput::GetPosition());
}
