#pragma once

#include "entity.h"

namespace og {
class Scene
{
public:
    virtual ~Scene()
    {
    }

    virtual EntityPtr AddEntity(EntityPtr a_e) = 0;

    virtual void OnMouseDown(const QPoint& a_point) = 0;

    virtual void OnMouseUp(const QPoint& a_point) = 0;
};
}
