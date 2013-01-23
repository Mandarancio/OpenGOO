#include "og_resourceconfig.h"

OGResourceConfig::OGResourceConfig(const QString & filename)
    : OGXmlConfig(filename)
{
    SetRootTag("ResourceManifest");
}

OGResources OGResourceConfig::Parser(QString groupid)
{
    QString defaultPath("./");
    QString defaultIdPrefix("");

    OGResources resources;

    for(QDomNode n=rootElement.firstChild(); !n.isNull(); n=n.nextSibling())
    {
        QDomElement element = n.toElement();

        if (element.tagName() == "Resources")
        {                        
            if (groupid.isEmpty() || element.attribute("id") == groupid)
            {               
                OGResourceGroup group;
                group.id = element.attribute("id");
                QDomNode node = element.firstChild();

                for(; !node.isNull(); node=node.nextSibling())
                {
                    QDomElement resElement = node.toElement();

                    if (resElement.tagName() == "SetDefaults")
                    {
                        defaultPath = resElement.attribute("path");
                        defaultIdPrefix = resElement.attribute("idprefix");
                    }
                    else if (resElement.tagName() == "Image")
                    {
                        OGResource res = {
                            OGResource::IMAGE,
                            defaultIdPrefix + resElement.attribute("id"),
                            defaultPath + resElement.attribute("path")
                        };

                        group.resource << res;
                    }
                    else if (resElement.tagName() == "Sound")
                    {
                        OGResource res = {
                            OGResource::SOUND,
                            defaultIdPrefix + resElement.attribute("id"),
                            defaultPath + resElement.attribute("path")
                        };

                        group.resource << res;
                    }
                    else if (resElement.tagName() == "font")
                    {
                        OGResource res = {
                            OGResource::FONT,
                            defaultIdPrefix + resElement.attribute("id"),
                            defaultPath + resElement.attribute("path")
                        };

                        group.resource << res;
                    }
                }

                resources.group << group;
            }            
        }
    }

    return resources;
}
