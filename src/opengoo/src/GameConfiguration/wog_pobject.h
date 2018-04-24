#pragma once

#include <QString>
#include <QPointF>

// The basic class of World of Goo physical objects

struct WOGPhysicsObject
{
    QString id;
    bool dynamic;
    QString tag;
    QString material;
    float mass;
    float x;
    float y;
    float rotation;

    WOGPhysicsObject()
        : dynamic(false)
        , tag(QLatin1String("walkable"))
        , mass(0)
        , x(0)
        , y(0)
        , rotation(0)
    {
    }

    virtual ~WOGPhysicsObject()
    {
    }
};
