#pragma once

#include "og_xmlconfig.h"
#include "wog_island.h"

class OGIslandConfig : public OGXmlConfig<DefaultTagParser>
{
public:
    OGIslandConfig()
    {
        SetRootTag("island");
    }

    WOGIsland* Parser();

private:
    QStringList StringToList(const QString & attribute);
    void StringToOCD(WOGOcd* ocd, const QString & attribute);
    wog::Level* CreateLevel(const QDomElement & element);
};
