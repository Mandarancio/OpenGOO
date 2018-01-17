#include "pointparticleemmiter.h"
#include <cmath>

#include <QDebug>

#include "particle.h"
#include "particlesystem.h"

namespace og
{
const float PointParticleEmmiter::MinRate = 0.00001;

void PointParticleEmmiter::SetRate(float aRate)
{
    mAccumulatedRate = 0;
    mRate.second = modf(aRate, reinterpret_cast<float*>(&mRate.first));
}

void PointParticleEmmiter::Update()
{
    mAccumulatedRate += mRate.second;
    if (mAccumulatedRate > (1 - MinRate))
    {
        mAccumulatedRate -= 1;
        const auto pos = GetParticleSystem()->GetPosition().toPointF();
        for (int i = 0; i < mRate.first + 1; ++i)
        {
            Emmit(pos);
        }
    }

    ParticleEmmiter::Update();
}

void PointParticleEmmiter::Render(QPainter& aPainter)
{
    foreach (auto particle, mUpdate)
    {
        particle->Render(aPainter);
    }
}
}
