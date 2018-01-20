#include "particleemiter.h"

#include "../og_gameengine.h"
#include "../../OGLib/util.h"

// TODO Should test:
//  dampening

namespace og
{
void ParticleEmiter::Emit(const QPointF& aPosition, const ParticleDefination aDef)
{
    auto particle = (GetCountParticles() + 1 > GetMaxPirticles()) ? PopParticle() : mParticlePool.Create();
    particle->SetPosition(aPosition);

    auto generator = GE->GetRandomGenerator();

    particle->SetAcceleration(aDef.acceleration);

    if (OptionalHasValue(aDef.dampening))
    {
        particle->SetDampening(OptionalValue(aDef.dampening) / GE->getFrameRate());
    }

    particle->SetImageSource(aDef.imageSources[generator->Range(0, static_cast<int>(aDef.imageSources.size()) - 1)]);

    if (aDef.isAdditive)
    {
        particle->SetEnabledAdditive(true);
    }

    {
        const auto dir = aDef.direction + generator->Range(-aDef.dirvar, aDef.dirvar);
        const auto sp = generator->Range(aDef.speed[0], aDef.speed[1]);
        particle->SetVelocity(sp, dir);
    }

    auto scale = generator->Range(aDef.scale[0], aDef.scale[1]);
    particle->SetScale(scale);

    if (aDef.isDirected)
    {
        particle->SetDirected(true);
    }
    else
    {
        particle->SetAngle(generator->Range(aDef.rotation.x(), aDef.rotation.y()));
        if (OptionalHasValue(aDef.rotationSpeed))
        {
            const auto rotSpeedInRad = generator->Range(OptionalValue(aDef.rotationSpeed).x()
                                                        , OptionalValue(aDef.rotationSpeed).y());
            particle->SetRotationSpeed(qRadiansToDegrees(rotSpeedInRad) / GE->getFrameRate());
        }
    }

    if (OptionalHasValue(aDef.lifespan))
    {
        const auto lifespan = generator->Range(OptionalValue(aDef.lifespan).x(),
                                               OptionalValue(aDef.lifespan).y());
        particle->SetEnabledLifespan(true);
        particle->SetLifespan(lifespan * GE->getFrameRate());

        if (OptionalHasValue(aDef.finalScale))
        {
            const auto finalScale = OptionalHasValue(aDef.finalScale);
            particle->SetFinalScale(finalScale);
            particle->SetScaleSpeed((finalScale - scale) / particle->GetLifespan());
        }

        if (aDef.shouldFade)
        {
            particle->SetFadeSpeed(1.0f / particle->GetLifespan());
        }
    }

    if (OptionalHasValue(aDef.xAxialSinOffset))
    {
        const auto& aso = OptionalValue(aDef.xAxialSinOffset);
        auto amp = generator->Range(aso.amp[0], aso.amp[1]);
        auto step = generator->Range(aso.freq[0], aso.freq[1]) / GE->getFrameRate();
        auto phase = generator->Range(aso.phaseshift[0], aso.phaseshift[1]);
        particle->SetAxialSinOffset(Particle::e_x, amp, step, phase);
    }

    if (OptionalHasValue(aDef.yAxialSinOffset))
    {
        const auto& aso = OptionalValue(aDef.yAxialSinOffset);
        auto amp = generator->Range(aso.amp[0], aso.amp[1]);
        auto step = generator->Range(aso.freq[0], aso.freq[1]) / GE->getFrameRate();
        auto phase = generator->Range(aso.phaseshift[0], aso.phaseshift[1]);
        particle->SetAxialSinOffset(Particle::e_y, amp, step, phase);
    }

    AddParticle(particle);
}

void ParticleEmiter::Emit(const QPointF& aPosition)
{
    auto generator = GE->GetRandomGenerator();
    const auto& def = mParticleDefinations[generator->Range(0, static_cast<int>(mParticleDefinations.size()) - 1)];
    Emit(aPosition, def);
}

void ParticleEmiter::Update()
{
    for (auto it = mUpdate.begin(); it != mUpdate.end();)
    {
        auto particle = *it;
        particle->Update();

        if (particle->GetShouldRelease())
        {
            mParticlePool.Release(particle);
            it = mUpdate.erase(it);
            continue;
        }

        ++it;
    }
}
}
