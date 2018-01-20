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
    AmbientParticleEmiter(int aMaxParticles, ParticleSystem* aSystem);

    ~AmbientParticleEmiter();

    void SetMargin(int aMargin)
    {
        mMargin = QMarginsF(aMargin, aMargin, aMargin, aMargin);
    }

    void SetTimeoutInterval(int aInterval)
    {
        mTimer.SetInterval(aInterval);
    }

private:
    void Update();

private:
    QMarginsF mMargin;
    Timer mTimer;
    std::unique_ptr<PositionGenerator<10>> mPositionGenerator;
};
}
