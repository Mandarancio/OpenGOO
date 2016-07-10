#include "cap.h"

Cap::Cap(float a_x, float a_y, GraphicList& a_list)
    : Entity(a_x, a_y)
{
    m_graphics[e_open] = a_list[e_open];
    m_graphics[e_closed] = a_list[e_closed];
    SetGraphic(m_graphics[e_closed]);

    m_isOpen = false;
}

void Cap::SetOpen(bool a_flag)
{
    if (a_flag)
    {
        SetGraphic(m_graphics[e_open]);
        m_isOpen = true;
    }
    else
    {
        SetGraphic(m_graphics[e_closed]);
        m_isOpen = false;
    }
}

