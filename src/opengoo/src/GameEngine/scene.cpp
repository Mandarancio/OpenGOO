#include "scene.h"

namespace og
{

void Scene::Update()
{
    for (auto it = m_update.begin(); it != m_update.end(); ++it)
    {
        (*it)->Update();
    }
}

void Scene::Render(QPainter& a_painter)
{
    for (auto it = m_render.begin(); it != m_render.end(); ++it)
    {
        (*it)->Render(a_painter);
    }
}

}
