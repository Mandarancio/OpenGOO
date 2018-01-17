#pragma once

#include "particleemmiter.h"

namespace og
{
class PointParticleEmmiter : public ParticleEmmiter
{
public:
    static const float MinRate;

public:
    PointParticleEmmiter(int aMaxParticles, ParticleSystem* aSystem)
        : ParticleEmmiter(aMaxParticles, aSystem)
    {
        SetRate(MinRate);
    }

    void SetRate(float aRate);

private:
    void Update();

    void Render(QPainter& aPainter);

private:
    std::pair<int, float> mRate;
    float mAccumulatedRate;
};
}
