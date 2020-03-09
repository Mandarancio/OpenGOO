#include "pointparticleemiter.h"
#include "particlesystem.h"

#include <cmath>

namespace og
{
const float PointParticleEmiter::MinRate = 0.00001;

void PointParticleEmiter::SetRate(float aRate)
{
    mAccumulatedRate = 0;
    float intpart;
    mRate.second = std::modf(aRate, &intpart);
    mRate.first = intpart;
}

void PointParticleEmiter::PreUpdate()
{
    mAccumulatedRate += mRate.second;
    if (mAccumulatedRate > (1 - MinRate))
    {
        mAccumulatedRate -= 1;
        const auto pos = GetParticleSystem()->GetPosition().toPointF();
        for (int i = 0; i < mRate.first + 1; ++i)
        {
            Emit(pos);
        }
    }
}
}
