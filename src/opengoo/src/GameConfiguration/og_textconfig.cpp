#include "og_textconfig.h"

OGTextConfig::OGTextConfig(const QString & filename)
    :OGXmlConfig(filename)
{
    SetRootTag("strings");
}

OGStringList OGTextConfig::Parser()
{
    OGStringList strings;

    for(QDomNode n=rootElement.firstChild(); !n.isNull(); n=n.nextSibling())
    {
        QDomElement domElement = n.toElement();

        if (domElement.tagName() == "string")
        {
            OGString str = {
                domElement.attribute("id", ""),
                domElement.attribute("text", ""),
                domElement.attribute("es", ""),
                domElement.attribute("fr", ""),
                domElement.attribute("de", ""),
                domElement.attribute("it", "")
            };

            strings << str;
        }
    }

    return strings;
}
