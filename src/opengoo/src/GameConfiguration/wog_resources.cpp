#include "wog_resources.h"

WOGResourceGroup::~WOGResourceGroup()
{
    while (!resource.isEmpty()) { delete resource.takeFirst(); }
}

WOGResources::~WOGResources()
{
    while (!group.isEmpty()) { delete group.takeFirst(); }
}

QString WOGResources::GetResource(
        WOGResource::Type type,
        const QString & id,
        const QString & groupid
        )
{
    for (int i=0; i<group.size(); i++)
    {
        if (groupid.isEmpty() || group.at(i)->id == groupid)
        {
            for (int j=0; j<group.at(i)->resource.size(); j++)
            {
                if (group.at(i)->resource.at(j)->type == type)
                {
                    if (group.at(i)->resource.at(j)->id == id)
                    {
                        return group.at(i)->resource.at(j)->path;
                    }
                }
            }
        }
    }

    return QString();
}
