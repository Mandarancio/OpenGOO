#include "particleemiter.h"

#include "../og_gameengine.h"

// TODO Should implement:
//  axialsinoffset

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

    if (aDef.dampening.first)
    {
        particle->SetEnabledDampening(true);
        particle->SetDampening(aDef.dampening.second / GE->getFrameRate());
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
        if (aDef.rotationSpeed.first)
        {
            const auto rotSpeedInRad = generator->Range(aDef.rotationSpeed.second.x(), aDef.rotationSpeed.second.y());
            particle->SetEnabledRotationSpeed(true);
            particle->SetRotationSpeed(qRadiansToDegrees(rotSpeedInRad) / GE->getFrameRate());
        }
    }

    if (aDef.lifespan.first)
    {
        const auto lifespan = generator->Range(aDef.lifespan.second.x(), aDef.lifespan.second.y());
        particle->SetEnabledLifespan(true);
        particle->SetLifespan(1 * GE->getFrameRate());

        if (aDef.finalScale.first)
        {
            const auto finalScale = aDef.finalScale.second;
            particle->SetEnabledFinalScale(true);
            particle->SetFinalScale(finalScale);
            particle->SetScaleSpeed((finalScale - scale) / particle->GetLifespan());
        }

        if (aDef.shouldFade)
        {
            particle->SetEnabledFade(true);
            particle->SetFadeSpeed(1.0f / particle->GetLifespan());
        }
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
