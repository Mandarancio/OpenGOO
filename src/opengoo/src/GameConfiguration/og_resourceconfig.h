#pragma once

#include "og_xmlconfig.h"

class WOGResources;
struct WOGResource;
struct WOGResourceGroup;

class OGResourceConfig : public OGXmlConfig
{
    QString defaultPath_;
    QString defaultIdPrefix_;
    QString mGroupId;

    enum ResourceType
    {
        Image,
        Sound,
        Font
    };

public:
    typedef WOGResources* Type;

public:
    OGResourceConfig(const QString& filename)
        : OGXmlConfig(filename)
    {
        SetRootTag("ResourceManifest");
    }

    WOGResources* Parser();

    void SetGroupId(const QString aGroupId)
    {
        mGroupId = aGroupId;
    }

private:
    WOGResource* CreateResource(const QDomElement& element, ResourceType aType);
    WOGResourceGroup* CreateResourceGroup(const QDomElement& element);
};
