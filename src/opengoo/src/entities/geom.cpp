#include "geom.h"

#include "GameConfiguration/Parsers/enumerator.h"

#include <QDebug>

template <>
struct Enumerator<Geom::Tag>
{
    static Geom::Tag ToEnum(const QString& aType)
    {
        if (aType == QLatin1String("walkable"))
        {
            return Geom::e_walkable;
        }
        if (aType == QLatin1String("detaching"))
        {
            return Geom::e_detaching;
        }
        if (aType == QLatin1String("stopsign"))
        {
            return Geom::e_stopsign;
        }
        if (aType == QLatin1String("unwalkable"))
        {
            return Geom::e_unwalkable;
        }
        if (aType == QLatin1String("deadly"))
        {
            return Geom::e_deadly;
        }

        qDebug() << aType;

        return Geom::e_unknown;
    }
};

Geom::Geom(float aX, float aY, const QString& aTag)
    : Entity(aX, aY)
    , mTag(Enumerator<Geom::Tag>::ToEnum(aTag))
{
}

bool Geom::IsLine() const
{
    return GetPhysicsBody()->GetShape().GetType() == og::physics::Shape::e_edge;
}
