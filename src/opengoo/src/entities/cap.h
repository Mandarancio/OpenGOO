#pragma once

#include "GameEngine/entity.h"

class Cap : public og::Entity
{   
    GraphicPtr m_graphics[2];
    bool m_isOpen;

public:
    enum
    {
        e_open,
        e_closed
    };

    Cap(float a_x, float a_y, GraphicList& a_list);

    void SetOpen(bool a_flag);
};
