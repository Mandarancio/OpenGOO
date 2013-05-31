#ifndef WOG_ISLAND_H
#define WOG_ISLAND_H

struct WOGOcd
{
    enum Type {BALLS, TIME, MOVES};

    Type type;
    int number;

    WOGOcd(): number(-1) { }
};

namespace wog {
struct Level
{
    QString id;
    QString name;
    QString text;
    WOGOcd ocd;
    QString oncomplete;
    QStringList cutscene;
};

} // namespace wog

struct WOGIsland
{
    QString name;
    QString map;
    QString icon;
    QList<wog::Level*> level;

    ~WOGIsland() { while (!level.isEmpty()) { delete level.takeFirst(); } }
};

#endif // WOG_ISLANDCONFIG_H
