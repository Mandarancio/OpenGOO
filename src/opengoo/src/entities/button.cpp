#include "GameEngine/input.h"
#include "GameEngine/og_gameengine.h"

#include "og_windowcamera.h"

#include "button.h"

Button::Button(float a_x, float a_y, GraphicPtr a_up, GraphicPtr a_over, const Callback& a_callback)
    : Entity(a_x, a_y, a_up)
    , m_up(a_up)
    , m_over(a_over)
    , m_callback(a_callback)
    , m_isOver(false)
{
}

void Button::Update()
{
    auto pos = QVector2D(GE->windowToLogical(og::MouseInput::GetPosition()));
    auto isOver = GetCollider()->OverlapPoint(pos);

    if (!m_isOver && isOver)
        SetGraphic(m_over);
    else if(m_isOver && !isOver)
        SetGraphic(m_up);

    m_isOver = isOver;
}
