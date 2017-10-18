#pragma once

#include "og_ibody.h"
#include "wog_scene.h"
#include <QPainter>

namespace og
{
namespace physics
{
class PhysicsEngine;
}
}

class OGRectangle : public OGIBody
{
    void _Draw(QPainter* p);

public:
    typedef WOGRectangle Data;

public:
    /// throws std::logic_error
    OGRectangle(og::physics::PhysicsEngine& a_physicEngine, const WOGRectangle& a_rect, const WOGMaterial& a_material);

    virtual ~OGRectangle()
    {
    }
};
