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

void OGMaterialConfig::Parser(const QDomElement &aElement, WOGMaterialList *obj)
{    
    QDomNode node;
    QDomElement element;

    node = aElement.firstChild();

    while(!node.isNull())
    {
        element = node.toElement();

        if (element.tagName() == "material")
        {
            obj->push_back(WOGMaterial());
            OGMaterialConfigHelper::LoadMaterial(obj->back(), element);
        }

        node = node.nextSibling();
    }
}
