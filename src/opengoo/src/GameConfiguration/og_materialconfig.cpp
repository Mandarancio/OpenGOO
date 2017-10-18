#include "og_materialconfig.h"

struct OGMaterialConfigHelper
{
    static void LoadMaterial(WOGMaterial& aMaterial, const QDomElement& element);
};

void OGMaterialConfigHelper::LoadMaterial(WOGMaterial& aMaterial, const QDomElement& element)
{
    aMaterial.bounce = element.attribute("bounce").toFloat();
    aMaterial.friction = element.attribute("friction").toFloat();
    aMaterial.id = element.attribute("id");
    aMaterial.minbouncevel = element.attribute("minbouncevel").toFloat();
    aMaterial.stickiness = element.attribute("stickiness").toInt();
}

WOGMaterialList OGMaterialConfig::Parser()
{    
    QDomNode node;
    QDomElement element;

    WOGMaterialList obj;
    node = rootElement.firstChild();

    while(!node.isNull())
    {
        element = node.toElement();

        if (element.tagName() == "material")
        {
            obj.push_back(WOGMaterial());
            OGMaterialConfigHelper::LoadMaterial(obj.back(), element);
        }

        node = node.nextSibling();
    }

    return obj;
}
