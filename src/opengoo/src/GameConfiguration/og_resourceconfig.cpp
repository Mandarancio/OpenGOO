#include "og_resourceconfig.h"

OGResourceConfig::OGResourceConfig(const QString & filename)
    : OGXmlConfig(filename)
{
    SetRootTag("ResourceManifest");
}

void OGResourceConfig::Parser(QList <OGResource > & resources)
{
    QDomElement res = rootElement .firstChildElement("Resources");

    for(QDomNode n = res.firstChild(); !n.isNull(); n = n.nextSibling())
    {
        QDomElement domElement = n.toElement();

        if (domElement.tagName() == "Image")
        {
            OGResource resource;
            resource.type = OGResource::IMAGE;
            resource.id = domElement.attribute("id", "");
            resource.path = domElement.attribute("path", "");
            resources << resource;
        }

        if (domElement.tagName() == "Sound")
        {
            OGResource resource;
            resource.type = OGResource::SOUND;
            resource.id = domElement.attribute("id", "");
            resource.path = domElement.attribute("path", "");
            resources << resource;
        }
    }
}

