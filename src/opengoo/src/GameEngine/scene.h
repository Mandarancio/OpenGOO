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
};
}
