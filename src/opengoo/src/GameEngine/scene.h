#pragma once

#include <list>
#include <map>

#include "entity.h"

namespace og {
namespace physics
{
class PhysicsEngine;
}

class IScene
{
public:
    virtual ~IScene()
    {
    }

    virtual void Update() = 0;

    virtual void Render(QPainter& a_painter) = 0;

    virtual EntityPtr AddEntity(EntityPtr a_entity) = 0;

    virtual EntityPtr RemoveEntity(EntityPtr a_entity) = 0;

    virtual void OnMouseDown(const QPoint& a_point) = 0;

    virtual void OnMouseUp(const QPoint& a_point) = 0;

    virtual void OnMouseMove(const QPoint& a_point) = 0;

    virtual int GetCount() const = 0;

    virtual const QString& GetName() const = 0;

    virtual physics::PhysicsEngine* GetPhysicsEngine() = 0;

    virtual float GetWidth() const = 0;

    virtual float GetHeight() const = 0;
};

class Scene : public IScene
{
    typedef std::list<EntityPtr> EntityPtrList;

public:
    Scene(const QString& a_name)
        : m_name(a_name)
    {
    }

    void Update();

    void Render(QPainter& a_painter);

    EntityPtr AddEntity(EntityPtr a_entity)
    {
        m_add.push_back(a_entity);
        return a_entity;
    }

    EntityPtr RemoveEntity(EntityPtr a_entity)
    {
        m_remove.push_back(a_entity);
        return a_entity;
    }

    void OnMouseDown(const QPoint& a_point);

    void OnMouseUp(const QPoint& a_point);

    void OnMouseMove(const QPoint& /*a_point*/)
    {
    }

    int GetCount() const
    {
        return 0;
    }

    const QString& GetName() const
    {
        return m_name;
    }

    physics::PhysicsEngine* GetPhysicsEngine()
    {
        return nullptr;
    }

    float GetWidth() const
    {
        return 0;
    }

    float GetHeight() const
    {
        return 0;
    }

private:

    QString m_name;
    EntityPtrList m_add;
    EntityPtrList m_remove;
    EntityPtrList m_update;
    std::map<float, EntityPtrList> m_render;
};
}
