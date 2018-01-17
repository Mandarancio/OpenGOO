#pragma once

#include "particleemmiter.h"

namespace og
{
class AmbientParticleEmmiter : public ParticleEmmiter
{
public:
    AmbientParticleEmmiter(int aMaxParticles, ParticleSystem* aSystem)
        : ParticleEmmiter(aMaxParticles, aSystem)
        , mMargin(50)
    {
    }

    void SetMargin(int aMargin)
    {
        mMargin = aMargin;
    }

private:
    void Update()
    {
    }

    void Render(QPainter& /*a_painter*/)
    {
    }

private:
    int mMargin;
};
}
