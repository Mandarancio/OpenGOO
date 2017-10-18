#pragma once

#include <QVector2D>

namespace og
{
class Entity;

class Collider
{
public:
    Collider(Entity* a_entity = nullptr) : m_entity(a_entity)
    {
    }

    virtual ~Collider()
    {
    }

    virtual bool OverlapPoint(const QVector2D& a_point) const = 0;

    void SetEntity(Entity* a_entity)
    {
        m_entity = a_entity;
    }

    const Entity& GetEntity() const
    {
        return *m_entity;
    }

private:
    Entity* m_entity;
};
}
