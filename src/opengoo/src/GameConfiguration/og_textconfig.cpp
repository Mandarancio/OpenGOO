#include "og_textconfig.h"

OGTextConfig::OGTextConfig(const QString & filename)
    :OGXmlConfig(filename)
{
    SetRootTag("strings");
}

void OGTextConfig::Parser(QList <OGText > & strings)
{
    for(QDomNode n = rootElement.firstChild(); !n.isNull(); n = n.nextSibling())
    {
        QDomElement domElement = n.toElement();

        if (domElement.tagName() == "string")
        {
            QString id = domElement.attribute("id", "");
            QString text = domElement.attribute("text", "");

            strings << OGText(id, text);
        }
    }
}
