
#include <QStringList>
#include "og_island.h"

using namespace og_islandconfig;

OGIslandConfig::OGIslandConfig(const QString & filename)
    : OGXmlConfig(filename)
{
    SetRootTag("island");
}

OGIsland OGIslandConfig::Parser()
{
    OGIsland island;

    island.name = rootElement.attribute("name");
    island.map = rootElement.attribute("map");
    island.icon = rootElement.attribute("icon");

    for (QDomNode n=rootElement.firstChild(); !n.isNull(); n=n.nextSibling())
    {
        QDomElement element = n.toElement();

        if (element.tagName() == "level")
        {
            OGLevel level = {
                element.attribute("id"),
                element.attribute("name"),
                element.attribute("text"),
                StringToOCD(element.attribute("ocd")),
                element.attribute("oncomplete"),
                StringToList(element.attribute("cutscene"))
            };

           island.level << level;
        }
    }

    return island;
}

QStringList OGIslandConfig::StringToList(const QString & attribute)
{
    QStringList list = attribute.split(",");

    if (list.size() == 3)
    {
        return list;
    }

    return QStringList();
}

OGOcd OGIslandConfig::StringToOCD(const QString & attribute)
{
    QStringList list = attribute.split(",");

    OGOcd::Type type = OGOcd::INVALID;

    if (list.size() == 2)
    {
        if (list.at(0) == "balls")
        {
            type =  OGOcd::BALLS;
        }
        else if (list.at(0) == "time")
        {
            type = OGOcd::TIME;
        }
        else if (list.at(0) == "moves")
        {
            type = OGOcd::MOVES;
        }

        OGOcd ocd;
        ocd.type   = type,
        ocd.number = list.at(1).toInt();


        return ocd;
    }

    return OGOcd();
}
