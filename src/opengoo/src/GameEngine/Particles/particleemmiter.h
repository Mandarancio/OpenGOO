#pragma once

#include "particlepool.h"

#include <list>
#include <array>

class QPainter;

namespace og
{
class ParticleSystem;
class ParticleEmmiter;

typedef std::shared_ptr<ParticleEmmiter> ParticleEmmiterSPtr;

struct ParticleDefination
{
    int direction;
    int dirvar;
    std::vector<ImageSourceSPtr> imageSources;
    std::array<float, 2> speed;
    std::array<float, 2> scale;
    std::pair<bool, float> finalScale;
    std::pair<bool, QPointF> lifespan;
    std::pair<bool, QPointF> rotationSpeed;
    std::pair<bool, float> dampening;
    bool shouldFade;
    bool isDirected;
    bool isAdditive;
    QPoint rotation;
    QPointF acceleration;

    ParticleDefination()
        : direction(0)
        , dirvar(0)
        , shouldFade(false)
        , isDirected(false)
        , isAdditive(false)
    {
        SetSpeed(QPointF(0, 0));
        SetScale(QPointF(1.0f, 1.0f));
        finalScale.first = false;
        lifespan.first = false;
        dampening.first = false;
    }

    void AddImageSource(ImageSourceSPtr& aSource)
    {
        imageSources.push_back(aSource);
    }

    void SetSpeed(const QPointF& aSpeed)
    {
        AssignValueToArray(speed, aSpeed);
    }

    void SetScale(const QPointF& aScale)
    {
        AssignValueToArray(scale, aScale);
    }

private:
    template<typename TArray, typename TValue>
    static void AssignValueToArray(TArray& aArray, const TValue& aVal)
    {
        assert(aArray.size() == 2);

        aArray[0] = aVal.x();
        aArray[1] = aVal.y();
    }
};

class ParticleEmmiter
{
public:
    enum Type
    {
        e_unknown,
        e_point,
        e_ambient,
        e_user
    };

    friend class ParticleSystem;

public:
    ParticleEmmiter(int aMaxParticles, ParticleSystem* aSystem)
        : mParticlePool(aMaxParticles)
        , mParticleSystem(aSystem)
    {
    }

    virtual ~ParticleEmmiter()
    {
    }

    virtual void Update() = 0;

    virtual void Render(QPainter& a_painter) = 0;

    Particle* AddParticle(Particle* aParticle)
    {
        mUpdate.push_back(aParticle);
        return aParticle;
    }

    void AddParticleDefination(ParticleDefination& aDefination)
    {
        mPartileDefinations.push_back(aDefination);
    }

    ParticleSystem* GetParticleSystem() const
    {
        return mParticleSystem;
    }

    int GetMaxPirticles() const
    {
        return mParticlePool.GetMaxParticleCount();
    }

    int CountParticles() const
    {
        return mUpdate.size();
    }

    Particle* PopParticle()
    {
        if (mUpdate.empty())
        {
            return nullptr;
        }

        Particle* p = mUpdate.front();
        p->Init();
        mUpdate.pop_front();
        return p;
    }

    void Emmit(const QPointF& aPosition);

protected:
    ParticlePool mParticlePool;
    std::list<Particle*> mUpdate;
    std::list<ParticleEmmiterSPtr>::iterator mIterator;
    ParticleSystem* mParticleSystem;
    std::vector<ParticleDefination> mPartileDefinations;
};
} // ns:og
