#pragma once

#include "../entity.h"
#include "particleemmiter.h"
#include "particleemmiterfactorymanager.h"

namespace og
{
class ParticleSystem : public Entity
{
    ParticleSystem(const QVector2D& aPosition, float aDepth);

public:
    ~ParticleSystem()
    {
    }

    static std::shared_ptr<ParticleSystem> Create(const QVector2D& aPosition, float aDepth)
    {
        return std::shared_ptr<ParticleSystem>(new ParticleSystem(aPosition, aDepth));
    }

    ParticleEmmiter* CreateEmmiter(ParticleEmmiter::Type aType, int aMaxParticles);

    void DestroyEmmiter(ParticleEmmiter* aEmmiter);

private:
    void Update();

    void Render(QPainter &a_painter);

private:
    std::list<ParticleEmmiterSPtr> mEmmiters;
    ParticleEmmiterFactoryManager mParticleEmmiterFactoryManager;
};
}
