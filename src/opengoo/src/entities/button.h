#pragma once

#include <functional>

#include "GameEngine/entity.h"

class Button : public og::Entity
{
    void Update();

    void OnMouseDown()
    {
        if (m_callback)
        {
            m_callback(*this);
        }
    }

public:
    typedef std::function<void(og::Entity&)> Callback;

    Button(float a_x, float a_y, GraphicPtr a_up, GraphicPtr a_over, const Callback& a_callback);

protected:
    virtual QVector2D GetMousePosition() const;

private:
    GraphicPtr m_up;
    GraphicPtr m_over;
    Callback m_callback;
    bool m_isOver;
};
