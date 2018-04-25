#pragma once

#include <QDomElement>
#include "wog_island.h"

// root tag: island
class OGIslandConfig
{
public:
    void Parser(const QDomElement& aElement, WOGIsland* obj);

private:
    QStringList StringToList(const QString & attribute);
    void StringToOCD(WOGOcd* ocd, const QString & attribute);
    wog::Level* CreateLevel(const QDomElement & element);
};
