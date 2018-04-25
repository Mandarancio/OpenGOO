#include "wog_resources.h"

bool WOGResourceGroup::CheckResourceType(WOGResource::Type aType) const
{
    switch (aType)
    {
    case WOGResource::FONT:
    case WOGResource::IMAGE:
    case WOGResource::SOUND:
        return true;
    }

    return false;
}

const WOGResource* WOGResourceGroup::FindResource(WOGResource::Type aType, const QString& aId) const
{
    if (aId.isEmpty() || !CheckResourceType(aType))
    {
        return nullptr;
    }

    const auto& rl = resource[aType];
    auto it = std::find_if(rl.cbegin(), rl.cend(),[&aId](const WOGResource& res)
    {
        return res.id == aId;
    });

    if (it != rl.cend())
    {
        return &*it;
    }

    return nullptr;
}

QString WOGResources::FindPath(WOGResource::Type aType, const QString& aResId) const
{
    for (auto it = group.cbegin(); it != group.cend(); ++it)
    {
        if (auto res = it->FindResource(aType, aResId))
        {
            return res->path;
        }
    }

    return QString();
}

QString WOGResources::FindPath(WOGResource::Type aType, const QString& aResId, const QString& aGroupId) const
{
    auto it = std::find_if(group.cbegin(), group.cend(), [&aGroupId](const WOGResourceGroup& grp)
    {
        return grp.id == aGroupId;
    });

    if (it != group.cend())
    {
        if (auto res = it->FindResource(aType, aResId))
        {
            return res->path;
        }
    }

    return QString();
}

QString WOGResources::GetResource(WOGResource::Type type, const QString& id, const QString& groupid) const
{
    return groupid.isEmpty() ? FindPath(type, id) : FindPath(type, id, groupid);
}
