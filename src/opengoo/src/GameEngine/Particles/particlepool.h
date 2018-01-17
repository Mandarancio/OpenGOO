#pragma once

#include "particle.h"
#include <stack>

namespace og
{
class Particle;

class ParticlePool
{
public:
    ParticlePool(int aMaxParticleCount)
        : mParticles(aMaxParticleCount)
        , mUsedParticleCount(0)
    {
        for (decltype(mParticles.size()) i = 0; i < mParticles.size(); ++i)
        {
            PushParticle(&mParticles[i]);
        }
    }

    int GetMaxParticleCount() const
    {
        return mParticles.size();
    }

    Particle* Create()
    {
        auto p = PopParticle();
        if (p)
        {
            p->Init();
            ++mUsedParticleCount;
        }

        return p;
    }

    void Release(Particle* aParticle)
    {
        PushParticle(aParticle);
        --mUsedParticleCount;
    }

    int GetUsedParticleCount() const
    {
        return mUsedParticleCount;
    }

private:
    void PushParticle(Particle* aParticle)
    {
         mStack.push(aParticle);
    }

    Particle* PopParticle()
    {
        if (mStack.empty())
        {
          return nullptr;
        }

        auto p = mStack.top();
        mStack.pop();
        return p;
    }

private:
    std::vector<Particle> mParticles;
    int mUsedParticleCount;
    std::stack<Particle*, std::vector<Particle*>> mStack;
};
}

