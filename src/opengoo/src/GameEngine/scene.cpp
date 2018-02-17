#include "sceneloader.h"

#include "scene.h"

namespace og
{

void Scene::Update()
{
    std::for_each(m_update.begin(), m_update.end(), [](EntityPtr& a_e) { a_e->Update(); });

    while (!m_add.empty())
    {
        auto entity = m_add.front();
        entity->m_update_iterator = m_update.insert(m_update.end(), entity);

        auto it = m_render.find(entity->GetDepth());
        if (it == m_render.end())
        {
            it = m_render.emplace(std::make_pair(entity->GetDepth(), EntityPtrList())).first;
        }

        auto& entryList = it->second;
        entity->m_render_iterator = entryList.insert(entryList.end(), entity);

        m_add.pop_front();
        entity->Added();
    }

    while (!m_remove.empty())
    {
        auto entity = m_remove.front();
        m_update.erase(entity->m_update_iterator);

        auto it = m_render.find(entity->GetDepth());
        if (it != m_render.end())
        {
            auto& entryList = it->second;
            entryList.erase(entity->m_render_iterator);
            if (entryList.empty())
            {
                m_render.erase(it);
            }
        }

        m_remove.pop_front();
        entity->Removed();
    }
}

void Scene::Render(QPainter& a_painter)
{
    for (auto it = m_render.begin(); it != m_render.end(); ++it)
    {
        auto& depthList = it->second;
        std::for_each(depthList.begin(), depthList.end(), [&a_painter](EntityPtr& a_e) { a_e->Render(a_painter); });
    }
}

void Scene::OnMouseDown(const QPoint& a_point)
{
    QVector2D point(a_point);
    for (auto it = m_update.cbegin(); it != m_update.cend(); ++it)
    {
        if (auto col = (*it)->GetCollider())
        {
            if (col->OverlapPoint(point))
            {
                (*it)->OnMouseDown();
                break;
            }
        }
    }
}

void Scene::OnMouseUp(const QPoint& a_point)
{
    QVector2D point(a_point);
    for (auto it = m_update.cbegin(); it != m_update.cend(); ++it)
    {
        if (auto col = (*it)->GetCollider())
        {
            if (col->OverlapPoint(point))
            {
                (*it)->OnMouseUp();
                break;
            }
        }
    }
}

void Scene::OnBegin()
{
    SceneLoader sl;
    sl.Load(*this);
}

} // ns:og
