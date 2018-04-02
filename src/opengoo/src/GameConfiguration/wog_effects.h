#pragma once

#include <QHash>
#include <QString>
#include <QPointF>
#include <QPoint>
#include <QStringList>

#include "GameEngine/Particles/particleemiter.h"

class WOGEffect
{
public:
    typedef og::ParticleEmiter::Type Type;

public:
    struct WOGParticle
    {
        struct WOGAxialSinOffset
        {
            enum Axis
            {
                e_x,
                e_y
            };

            Axis axis;
            QPointF freq;
            QPoint amp;
            QPointF phaseshift;
        };

        QStringList image;
        bool additive;
        bool directed;
        bool fade;
        QPointF scale;
        QPointF speed;
        QPointF acceleration;
        int movedir;
        int movedirvar;
        QPoint rotation;

        std::pair<bool, float> dampening;
        std::pair<bool, QPointF> lifespan;
        std::pair<bool, float> finalscale;
        std::pair<bool, QPointF> rotspeed;

        std::vector<WOGAxialSinOffset> axialsinoffset;

        WOGParticle()
            : additive(false)
            , directed(false)
            , fade(false)
            , movedir(0)
        {
            dampening.first = false;
            lifespan.first = false;
            finalscale.first = false;
            rotspeed.first = false;
        }
    };

private:
    Type mType;

public:
    WOGEffect(Type aType)
        : mType(aType)
        , margin(50)
        , maxparticles(1)
        , rate(0.00001)
    {
    }

    Type GetType() const
    {
        return mType;
    }

public:
    int margin;
    int maxparticles;
    float rate;
    std::vector<WOGParticle> particle;
};

struct WOGEffects : public QHash<QString, std::shared_ptr<WOGEffect>>
{
    static std::unique_ptr<WOGEffects> Create()
    {
        return std::unique_ptr<WOGEffects>(new WOGEffects);
    }
};
