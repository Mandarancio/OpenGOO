#pragma once

#include "GameEngine/entity.h"

class Geom : public og::Entity
{
public:
    enum Tag
    {
        e_walkable,
        e_detaching,
        e_stopsign,
        e_unwalkable,
        e_deadly,
        e_unknown
    };

public:
    Geom(float aX, float aY, const QString& aTag);

    Tag GetTag() const
    {
        return mTag;
    }

    bool IsLine() const;

private:
    Tag mTag;
};
