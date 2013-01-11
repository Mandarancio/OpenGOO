#ifndef OG_ISLANDCONFIG_H
#define OG_ISLANDCONFIG_H

#include <QList>
#include "og_xmlconfig.h"

namespace og_islandconfig
{
    struct OGLevel;
    struct OGIsland;
    typedef QList<OGLevel> OGLevelList;
    typedef QList<QString> OGStringList;
    struct OGOcd
    {
        enum Type {INVALID, BALLS, TIME, MOVES};
        Type type;
        int number;
        OGOcd():type(INVALID),number(-1){}
    };

    struct OGLevel
    {
        QString id;
        QString name;
        QString text;
        OGOcd ocd;
        QString oncomplete;
        OGStringList cutscene;
    };

    struct OGIsland
    {
        QString name;
        QString map;
        QString icon;
        OGLevelList level;
    };

    class OGIslandConfig : public OGXmlConfig
    {
    public:
        OGIslandConfig(const QString & filename);

        OGIsland Parser();

    private:
        QStringList StringToList(const QString & attribute);
        OGOcd StringToOCD(const QString & attribute);

    };

}
#endif // OG_ISLANDCONFIG_H
