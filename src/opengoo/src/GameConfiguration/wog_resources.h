#pragma once

#include <list>

#include <QString>

struct WOGResource
{
    enum Type
    {
        IMAGE,
        SOUND,
        FONT
    };

    Type type;
    QString id;
    QString path;
    QString jp;

    WOGResource()
    {
    }

    WOGResource(Type aType)
        : type(aType)
    {
    }
};

struct WOGResourceGroup
{
    typedef std::list<WOGResource> ResourceList;

    QString id;
    ResourceList resource[3];

    const WOGResource* FindResource(WOGResource::Type aType, const QString& aId) const;

    void AddResource(WOGResource&& aRes)
    {
        if (CheckResourceType(aRes.type))
        {
            resource[aRes.type].push_back(aRes);
        }
    }

    WOGResource* CreateImage()
    {
        return Create(WOGResource::IMAGE);
    }

    WOGResource* CreateSound()
    {
        return Create(WOGResource::SOUND);
    }

    WOGResource* CreateFont()
    {
        return Create(WOGResource::FONT);
    }

    WOGResource* Create(WOGResource::Type aType)
    {
        if (CheckResourceType(aType))
        {
            resource[aType].push_back(WOGResource(aType));
            return &resource[aType].back();
        }

        return nullptr;
    }

private:
    bool CheckResourceType(WOGResource::Type aType) const;
};

class WOGResources
{
public:
    std::list<WOGResourceGroup> group;

    QString GetResource(WOGResource::Type type, const QString& id, const QString& groupid=QString()) const;

    QString GetImage(const QString& id, const QString& groupid=QString()) const
    {
        return GetResource(WOGResource::IMAGE, id, groupid);
    }

    QString GetSound(const QString& id, const QString& groupid=QString()) const
    {
        return GetResource(WOGResource::SOUND, id, groupid);
    }

    void AddGroup(WOGResourceGroup&& aGrp)
    {
        group.push_back(aGrp);
    }

    static std::unique_ptr<WOGResources> Create()
    {
        return std::unique_ptr<WOGResources>(new WOGResources);
    }

private:
    QString FindPath(WOGResource::Type aType, const QString& aResId) const;

    QString FindPath(WOGResource::Type aType, const QString& aResId, const QString& aGroupId) const;
};
