#pragma once

#include <array>

#include "../timer.h"

#include "particleemiter.h"

namespace og
{
class AmbientParticleEmiter : public ParticleEmiter
{
    template<int Precision> class PositionGenerator;

public:
    AmbientParticleEmiter(int aMargin, int aTimeoutInterval, int aMaxParticles, ParticleSystem* aSystem);

    ~AmbientParticleEmiter();

    void SetMargin(int aMargin)
    {
        mMargin = QMarginsF(aMargin, aMargin, aMargin, aMargin);
    }

    void SetTimeoutInterval(int aInterval)
    {
        mTimer.SetInterval(aInterval);
    }

    static std::shared_ptr<AmbientParticleEmiter> Create(int aMargin, int aTimeoutInterval,
                                                         int aMaxParticles, ParticleSystem* aSystem);

private:
    void PreUpdate();

    void PostUpdate();

private:
    QMarginsF mMargin;
    Timer mTimer;
    std::unique_ptr<PositionGenerator<10>> mPositionGenerator;
};
}
