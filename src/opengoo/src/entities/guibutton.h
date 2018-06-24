#pragma once

#include "button.h"

class GUIButton : public Button
{
public:
    GUIButton(std::shared_ptr<og::Collider> aCollider, float aX, float aY, GraphicPtr aUp, GraphicPtr aOver,
              const Callback& aCallback);

private:
    void Render(QPainter& aPainer);

    virtual QVector2D GetMousePosition() const;
};
