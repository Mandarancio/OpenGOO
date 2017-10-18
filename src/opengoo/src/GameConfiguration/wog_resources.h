#pragma once

#include <QString>
#include <QList>

struct WOGResource
{
    enum Type {IMAGE, SOUND, FONT};

    Type type;
    QString id;
    QString path;
};

struct WOGResourceGroup
{
    QString id;
    QList<WOGResource*> resource;

    ~WOGResourceGroup();
};

class OGResourceConfig;

class WOGResources
{
public:
    typedef OGResourceConfig Conf;

public:
    QList<WOGResourceGroup*> group;

    QString GetResource(WOGResource::Type type, const QString& id
                        , const QString& groupid=QString()) const;

    QString GetImage(const QString& id, const QString& groupid=QString()) const
    {
        return GetResource(WOGResource::IMAGE, id, groupid);
    }

    QString GetSound(const QString& id, const QString& groupid=QString()) const
    {
        return GetResource(WOGResource::SOUND, id, groupid);
    }

    ~WOGResources();
};
