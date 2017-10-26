#pragma once

#include <QHash>
#include <QString>
#include <QPointF>
#include <QPoint>
#include <QStringList>

class WOGEffect
{
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

        QPointF scale;
        float finalscale;
        int movedirvar;
        QStringList image;
        float directed;
        int movedir;
        QPointF acceleration;
        bool additive;
        QPointF lifespan;
        QPointF speed;
        float dampening;
        QPoint rotation;
        QPointF rotspeed;
        bool fade;
        std::vector<WOGAxialSinOffset> axialsinoffset;
    };

    enum Type
    {
        e_unknown,
        e_point,
        e_ambient,
    };

private:
    Type mType;

public:
    WOGEffect(Type aType)
        : mType(aType)
        , margin(0)
        , maxparticles(0)
        , rate(0)
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

typedef QHash<QString, std::shared_ptr<WOGEffect>> WOGEffects;
