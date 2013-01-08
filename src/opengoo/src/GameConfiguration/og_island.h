#ifndef OG_ISLANDCONFIG_H
#define OG_ISLANDCONFIG_H

#include "og_xmlconfig.h"

namespace og_islandconfig
{
    typedef QList<OGLevel> OGLevelList;
    typedef QList<QString> OGStringList;

    struct OGOcd
    {
        enum Type {BALLS, TIME, MOVES};

        Type type;
        int number;
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
        OGStringList StringToList(const QString & attribute);
        OGOcd StringToOCD(const QString & attribute);

    };
}
#endif // OG_ISLANDCONFIG_H
