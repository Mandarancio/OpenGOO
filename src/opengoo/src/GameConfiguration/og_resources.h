#ifndef OG_RESOURCES_H
#define OG_RESOURCES_H

#include <QString>
#include <QList>

struct OGResource
{
    enum Type {IMAGE, SOUND, FONT};

    Type type;
    QString id;
    QString path;
};

struct OGResourceGroup
{
    QString id;
    QList<OGResource> resource;
};

class OGResources
{
public:
    QList<OGResourceGroup> group;

    QString GetResource(
            OGResource::Type type,
            const QString & id,
            const QString & groupid=QString()
            );
};

#endif // OG_RESOURCES_H
