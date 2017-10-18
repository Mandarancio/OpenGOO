#pragma once

#include "og_ibody.h"

struct WOGCircle;
class QPainter;

class OGCircle : public OGIBody
{
    void _Draw(QPainter* p);

public:
    typedef WOGCircle Data;

public:
    /// throws std::logic_error
    OGCircle(og::physics::PhysicsEngine& a_physicEngine, const WOGCircle& a_circle, const WOGMaterial& a_material);

    virtual ~OGCircle()
    {
    }


};
