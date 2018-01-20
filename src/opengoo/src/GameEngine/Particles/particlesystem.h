#pragma once

#include "../entity.h"
#include "particleemiter.h"
#include "particleemiterfactorymanager.h"

namespace og
{
class ParticleSystem : public Entity
{
    ParticleSystem(const QVector2D& aPosition, float aDepth);

public:
    static std::shared_ptr<ParticleSystem> Create(const QVector2D& aPosition, float aDepth)
    {
        return std::shared_ptr<ParticleSystem>(new ParticleSystem(aPosition, aDepth));
    }

    ParticleEmiter* CreateEmiter(ParticleEmiter::Type aType, int aMaxParticles);

    void DestroyEmiter(ParticleEmiter* aEmiter);

private:
    void Update();

    void Render(QPainter& aPainter);

private:
    std::list<ParticleEmiterSPtr> mEmiters;
    ParticleEmiterFactoryManager mParticleEmiterFactoryManager;
};
}
