#pragma once

#include "particleemiter.h"

namespace og
{
class PointParticleEmiter : public ParticleEmiter
{
public:
    static const float MinRate;

public:
    PointParticleEmiter(int aMaxParticles, ParticleSystem* aSystem)
        : ParticleEmiter(aMaxParticles, aSystem)
    {
        SetRate(MinRate);
    }

    void SetRate(float aRate);

private:
    void Update();

private:
    std::pair<int, float> mRate;
    float mAccumulatedRate;
};
}
