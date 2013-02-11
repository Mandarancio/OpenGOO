#include "og_materialconfig.h"

#include <QDebug>

OGMaterialConfig::OGMaterialConfig(const QString & filename)
    : OGXmlConfig(filename)
{
    SetRootTag("materials");
}

WOGMaterialList* OGMaterialConfig::Parser()
{    
    QDomNode node;
    QDomElement element;

    WOGMaterialList* obj = new WOGMaterialList;
    node = rootElement.firstChild();

    while(!node.isNull())
    {
        element = node.toElement();

        if (element.tagName() == "material")
        {
            obj->material << CreateMaterial(element);
        }

        node = node.nextSibling();
    }

    return obj;
}

WOGMaterial* OGMaterialConfig::CreateMaterial(const QDomElement & element)
{
    WOGMaterial* obj = new WOGMaterial;
    obj->bounce = element.attribute("bounce").toFloat();
    obj->friction = element.attribute("friction").toFloat();
    obj->id = element.attribute("id");
    obj->minbouncevel = element.attribute("minbouncevel").toFloat();
    obj->stickiness = element.attribute("stickiness").toInt();

    return obj;
}
