#pragma once

#include "particleemiter.h"

namespace og
{
class PointParticleEmiter : public ParticleEmiter
{
public:
    static const float MinRate;

public:
    PointParticleEmiter(float aRate, int aMaxParticles, ParticleSystem* aSystem)
        : ParticleEmiter(aMaxParticles, aSystem)
    {
        SetRate(aRate);
    }

    void SetRate(float aRate);

private:
    void PreUpdate();

private:
    std::pair<int, float> mRate;
    float mAccumulatedRate;
};
}
