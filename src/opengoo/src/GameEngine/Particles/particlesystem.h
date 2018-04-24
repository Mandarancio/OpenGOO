#pragma once

#include "../entity.h"
#include "particleemiter.h"

namespace og
{
struct IParticleEmiterFactory;

class ParticleSystem : public Entity
{
public:
    ParticleSystem(const QVector2D& aPosition, float aDepth, std::shared_ptr<IParticleEmiterFactory> aFactory)
        : Entity(aPosition)
        , mFactory(aFactory)
    {
        SetDepth(aDepth);
    }

    ParticleEmiter* CreateEmiter(const ParticleEmiterDefination& aDef);

    void DestroyEmiter(ParticleEmiter* aEmiter);

private:
    void Update();

    void Render(QPainter& aPainter);

private:
    std::list<std::shared_ptr<ParticleEmiter>> mEmiters;
    std::shared_ptr<IParticleEmiterFactory> mFactory;
};
}
