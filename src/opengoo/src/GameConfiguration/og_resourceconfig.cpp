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
            QString id = domElement.attribute("id", "");
            QString path = domElement.attribute("path", "");

            resources << OGResource(OGResource::IMAGE, id, path);
        }
        else if (domElement.tagName() == "Sound")
        {
            QString id = domElement.attribute("id", "");
            QString path = domElement.attribute("path", "");

            resources << OGResource(OGResource::SOUND, id, path);
        }
        else if (domElement.tagName() == "font")
        {
            QString id = domElement.attribute("id", "");
            QString path = domElement.attribute("path", "");

            resources << OGResource(OGResource::FONT, id, path);
        }
    }
}

