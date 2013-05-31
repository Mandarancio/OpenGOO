#ifndef OG_ISLANDCONFIG_H
#define OG_ISLANDCONFIG_H

#include "og_xmlconfig.h"
#include "wog_island.h"

class OGIslandConfig : public OGXmlConfig
{
public:
    OGIslandConfig(const QString & filename);

    WOGIsland* Parser();

private:
    QStringList StringToList(const QString & attribute);
    void StringToOCD(WOGOcd* ocd, const QString & attribute);
    wog::Level* CreateLevel(const QDomElement & element);

};

#endif // OG_ISLANDCONFIG_H
