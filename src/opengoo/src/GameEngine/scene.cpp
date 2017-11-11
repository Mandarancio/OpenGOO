#include "scene.h"

namespace og
{

void Scene::Update()
{
    std::for_each(m_update.begin(), m_update.end(), [](EntityPtr& a_e) { a_e->Update(); });

    if (!m_add.empty())
    {
        std::for_each(
            m_add.begin(),
            m_add.end(),
            [this](EntityPtr& a_e)
            {
                a_e->update_iterator = m_update.emplace(m_update.end(), a_e);

                auto it = m_render.find(a_e->GetDepth());
                if (it == m_render.end())
                {
                    it = m_render.emplace(std::make_pair(a_e->GetDepth(), std::list<EntityPtr>())).first;
                }

                a_e->render_iterator = it->second.emplace(it->second.end(), a_e);
            });

        m_add.clear();
    }

    if (!m_remove.empty())
    {
        std::for_each(
            m_remove.begin(),
            m_remove.end(),
            [this](EntityPtr& a_e)
            {
                m_update.erase(a_e->update_iterator);

                auto it = m_render.find(a_e->GetDepth());
                if (it != m_render.end())
                {
                    it->second.erase(a_e->render_iterator);
                    if (it->second.empty())
                    {
                        m_render.erase(it);
                    }
                }
            });

        m_remove.clear();
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

} // ns:og
