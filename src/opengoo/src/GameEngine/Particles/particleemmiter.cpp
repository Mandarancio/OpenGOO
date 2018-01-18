#include "particleemmiter.h"

#include "../og_gameengine.h"

// TODO Should implement:
//  axialsinoffset

// TODO Should test:
//  dampening

namespace og
{
void ParticleEmmiter::Emmit(const QPointF& aPosition)
{
    auto particle = (CountParticles() + 1 > GetMaxPirticles()) ? PopParticle() : mParticlePool.Create();
    particle->SetPosition(aPosition);

    auto generator = GE->GetRandomGenarator();
    const auto& def = mPartileDefinations[generator->Range(0, static_cast<int>(mPartileDefinations.size()) - 1)];

    particle->SetAcceleration(def.acceleration);

    if (def.dampening.first)
    {
        particle->SetEnabledDampening(true);
        particle->SetDampening(def.dampening.second / GE->getFrameRate());
    }

    particle->SetImageSource(def.imageSources[generator->Range(0, static_cast<int>(def.imageSources.size()) - 1)]);

    if (def.isAdditive)
    {
        particle->SetEnabledAdditive(true);
    }

    {
        const auto dir = def.direction + generator->Range(-def.dirvar, def.dirvar);
        const auto sp = generator->Range(def.speed[0], def.speed[1]);
        particle->SetVelocity(sp, dir);
    }

    auto scale = generator->Range(def.scale[0], def.scale[1]);
    particle->SetScale(scale);

    if (def.isDirected)
    {
        particle->SetDirected(true);
    }
    else
    {
        particle->SetAngle(generator->Range(def.rotation.x(), def.rotation.y()));
        if (def.rotationSpeed.first)
        {
            const auto rotSpeedInRad = generator->Range(def.rotationSpeed.second.x(), def.rotationSpeed.second.y());
            particle->SetEnabledRotationSpeed(true);
            particle->SetRotationSpeed(qRadiansToDegrees(rotSpeedInRad) / GE->getFrameRate());
        }
    }

    if (def.lifespan.first)
    {
        const auto lifespan = generator->Range(def.lifespan.second.x(), def.lifespan.second.y());
        particle->SetEnabledLifespan(true);
        particle->SetLifespan(lifespan * GE->getFrameRate());

        if (def.finalScale.first)
        {
            const auto finalScale = def.finalScale.second;
            particle->SetEnabledFinalScale(true);
            particle->SetFinalScale(finalScale);
            particle->SetScaleSpeed((finalScale - scale) / particle->GetLifespan());
        }

        if (def.shouldFade)
        {
            particle->SetEnabledFade(true);
            particle->SetFadeSpeed(1.0f / particle->GetLifespan());
        }
    }

    AddParticle(particle);
}

void ParticleEmmiter::Update()
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
