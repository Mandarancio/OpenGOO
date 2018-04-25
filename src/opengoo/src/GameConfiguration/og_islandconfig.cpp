#include "og_islandconfig.h"

using namespace wog;

void OGIslandConfig::Parser(const QDomElement& aElement, WOGIsland* obj)
{
    obj->name = aElement.attribute("name");
    obj->map = aElement.attribute("map");
    obj->icon = aElement.attribute("icon");

    QDomNode node = aElement.firstChild();

    while (!node.isNull())
    {
        QDomElement element = node.toElement();

        if (element.tagName() == "level")
        {           
            obj->level << CreateLevel(element);
        }

        node = node.nextSibling();
    }
}

Level* OGIslandConfig::CreateLevel(const QDomElement & element)
{
    Level* obj;

    obj = new Level;
    obj->id = element.attribute("id");
    obj->name = element.attribute("name");
    obj->text = element.attribute("text");
    StringToOCD(&obj->ocd, element.attribute("ocd"));
    obj->oncomplete = element.attribute("oncomplete");
    obj->cutscene = StringToList(element.attribute("cutscene"));

    return obj;
}

QStringList OGIslandConfig::StringToList(const QString & attribute)
{
    QStringList list = attribute.split(",");

    if (list.size() == 3) { return list; }

    return QStringList();
}

void OGIslandConfig::StringToOCD(WOGOcd* ocd, const QString & attribute)
{
    QStringList list = attribute.split(",");

    if (list.size() == 2)
    {
        if (list.at(0) == "balls")
        {
            ocd->type =  WOGOcd::BALLS;
            ocd->number = list.at(1).toInt();
        }
        else if (list.at(0) == "time")
        {
            ocd->type = WOGOcd::TIME;
            ocd->number = list.at(1).toInt();
        }
        else if (list.at(0) == "moves")
        {
            ocd->type = WOGOcd::MOVES;
            ocd->number = list.at(1).toInt();
        }        
    }
}
