#pragma once

#include <QPainter>
#include <QVector2D>

#include "graphic.h"

#include "og_types.h"
#include "Colliders/collider.h"
#include "PhysicsEngine/og_physicsbody.h"

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
    std::unique_ptr<og::PhysicsBody> mPhysicsBody;

    Entity(const Entity&);
    Entity& operator=(const Entity&);

public:
    typedef std::list<std::shared_ptr<Entity>>::iterator Iterator;

private:
    Iterator m_render_iterator;
    Iterator m_update_iterator;

    friend class Scene;

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

    virtual void LastUpdate()
    {
    }

    virtual void Render(QPainter& a_painter);

    Graphic* GetGraphic() const
    {
        return m_graphic.get();
    }

    void SetGraphic(GraphicPtr a_g)
    {
        if (a_g)
        {
            SetVisible(true);
            m_graphic = a_g;
        }
    }

    void SetVisible(bool a_flag)
    {
        m_visible = a_flag;
    }

    bool GetVisible() const
    {
        return m_visible;
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

    virtual bool OnMouseMove(const QPoint& /*aPosition*/)
    {
        return false;
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

    void SetPosition(const QPointF& a_position)
    {
        SetPosition(QVector2D(a_position));
    }

    void SetPosition(float aX, float aY)
    {
        m_position.setX(aX);
        m_position.setY(aY);
    }

    float GetX() const
    {
        return m_position.x();
    }

    float GetY() const
    {
        return m_position.y();
    }

    void SetX(float aX)
    {
        m_position.setX(aX);
    }

    void SetY(float aY)
    {
        m_position.setY(aY);
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

    void SetPhysicsBody(std::unique_ptr<PhysicsBody> aPhysicsBody)
    {
        mPhysicsBody = std::move(aPhysicsBody);
        mPhysicsBody->SetUserData(this);
    }

    PhysicsBody* GetPhysicsBody() const
    {
        return mPhysicsBody.get();
    }

    void DestroyPhysicsBody();

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
