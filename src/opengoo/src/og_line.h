#pragma once

#include "og_ibody.h"
#include "wog_scene.h"

class OGLine : public OGIBody
{
public:
    typedef WOGLine Data;

public:
    /// throws std::logic_error
    OGLine(og::physics::PhysicsEngine& a_physicEngine, const WOGLine& line, const WOGMaterial& material);

    virtual ~OGLine()
    {
    }
};
