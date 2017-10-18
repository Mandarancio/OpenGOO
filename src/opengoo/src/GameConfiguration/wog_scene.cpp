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

    while (!compositegeom.isEmpty()) { delete compositegeom.takeFirst(); }
}

WOGButtonGroup* WOGScene::GetButtonGroup(const QString& id)
{
    Q_FOREACH(WOGButtonGroup * group, buttongroup)
    {
        if (group->id.compare(id) == 0) { return group; }
    }

    return 0;
}

WOGButton* WOGScene::FindButton(const QString &id)
{
    WOGButton* btn = 0;

    Q_FOREACH (WOGButton* b, button)
    {
        if (b->id == id)
        {
            btn = b;
            break;
        }
    }

    return btn;
}
