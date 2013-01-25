#include "wog_scene.h"

WOGButtonGroup::~WOGButtonGroup()
{
    while (!button.isEmpty()) { delete button.takeFirst(); }
}

WOGScene::~WOGScene()
{
    while (!button.isEmpty()) { delete button.takeFirst(); }

    while (!buttongroup.isEmpty()) { delete buttongroup.takeFirst(); }

    while (!sceneLayer.isEmpty()) { delete sceneLayer.takeFirst(); }

    while (!label.isEmpty()) { delete label.takeFirst(); }

    while (!circle.isEmpty()) { delete circle.takeFirst(); }

    while (!line.isEmpty()) { delete line.takeFirst(); }

    while (!rectangle.isEmpty()) { delete rectangle.takeFirst(); }

    while (!linearforcefield.isEmpty())
    {
        delete linearforcefield.takeFirst();
    }

    while (!radialforcefield.isEmpty())
    {
        delete radialforcefield.takeFirst();
    }

    while (!particle.isEmpty()) { delete particle.takeFirst(); }
}
