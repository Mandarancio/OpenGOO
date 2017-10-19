#pragma once

#include <QPainter>
#include <QVector2D>

#include "graphic.h"

#include "og_types.h"
#include "Colliders/collider.h"

namespace og
{
class Scene;

class Entity
{
    QVector2D m_position;
    GraphicPtr m_graphic;
    std::shared_ptr<Collider> m_collider;
    Scene* m_scene;
    bool m_visible;
    QString m_name;
    float m_depth;

    Entity(const Entity&);
    Entity& operator=(const Entity&);

public:
    Entity(const QVector2D& a_position, GraphicPtr a_graphic = nullptr)
        : m_position(a_position)
        , m_graphic(a_graphic)
    {
        Init();
    }

    Entity(float a_x, float a_y, GraphicPtr a_graphic = nullptr)
        : m_position(a_x, a_y)
        , m_graphic(a_graphic)
    {
        Init();
    }

    virtual ~Entity()
    {
    }

    virtual void Update()
    {
    }

    virtual void Render(QPainter& a_painter)
    {
        if (m_visible)
            m_graphic->Render(a_painter, m_position);
    }

    GraphicPtr GetGraphic() const
    {
        return m_graphic;
    }

    void SetGraphic(GraphicPtr a_g)
    {
        SetVisible(true);
        m_graphic = a_g;
    }

    void SetVisible(float a_flag)
    {
        m_visible = a_flag;
    }

    virtual void Added()
    {
    }

    virtual void Removed()
    {
    }

    virtual void OnTriggerEnter(Fixture* /*a_fixture*/)
    {
    }

    virtual void OnTriggerExit(Fixture* /*a_fixture*/)
    {
    }

    virtual void OnMouseDown()
    {
    }

    virtual void OnMouseUp()
    {
    }

    void SetScene(Scene* a_scene)
    {
        m_scene = a_scene;
    }

    Scene* GetScene() const
    {
        return m_scene;
    }

    const Collider* GetCollider() const
    {
        return m_collider.get();
    }

    void SetCollider(std::shared_ptr<Collider> a_collider)
    {
        m_collider = a_collider;
        m_collider->SetEntity(this);
    }

    void SetPosition(const QVector2D& a_position)
    {
        m_position = a_position;
    }

    const QVector2D& GetPosition() const
    {
        return m_position;
    }

    void SetName(const QString& a_name)
    {
        m_name = a_name;
    }

    const QString& GetName() const
    {
        return m_name;
    }

    void SetDepth(float a_depth)
    {
        m_depth = a_depth;
    }

    float GetDepth() const
    {
        return m_depth;
    }

private:
    void Init()
    {
        m_visible = m_graphic ? true : false;
        m_scene = nullptr;
        m_depth = 0.0f;
    }
};
}

typedef std::shared_ptr<og::Entity> EntityPtr;


