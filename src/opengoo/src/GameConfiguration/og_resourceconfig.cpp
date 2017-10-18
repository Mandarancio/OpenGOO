#include "og_resourceconfig.h"
#include "wog_resources.h"

WOGResource* OGResourceConfig::CreateResource(const QDomElement& element, ResourceType aType)
{
    auto obj = std::unique_ptr<WOGResource>(new WOGResource);
    obj->id = defaultIdPrefix_ + element.attribute("id");
    obj->path = defaultPath_ + element.attribute("path");

    switch (aType)
    {
    case Image:
        obj->type = WOGResource::IMAGE;
        break;
    case Sound:
        obj->type = WOGResource::SOUND;
        break;
    case Font:
        obj->type = WOGResource::FONT;
        break;
    }

    return obj.release();
}

WOGResourceGroup* OGResourceConfig::CreateResourceGroup(const QDomElement& element)
{
    QDomNode node;
    QDomElement resElement;

    auto obj = std::unique_ptr<WOGResourceGroup>(new WOGResourceGroup);
    obj->id = element.attribute("id");
    node = element.firstChild();

    while(!node.isNull())
    {
        resElement = node.toElement();

        if (resElement.tagName() == "SetDefaults")
        {
            defaultPath_ = resElement.attribute("path") + "/";
            defaultIdPrefix_ = resElement.attribute("idprefix");
        }
        else if (resElement.tagName() == "Image")
        {
            obj->resource << CreateResource(resElement, Image);
        }
        else if (resElement.tagName() == "Sound")
        {
            obj->resource << CreateResource(resElement, Sound);
        }
        else if (resElement.tagName() == "font")
        {
            obj->resource << CreateResource(resElement, Font);
        }

        node = node.nextSibling();
    }

    return obj.release();
}

WOGResources* OGResourceConfig::Parser()
{
    QDomNode node;
    QDomElement element;

    defaultPath_ = "./";
    defaultIdPrefix_ = "";
    auto resources = std::unique_ptr<WOGResources>(new WOGResources);
    node = rootElement.firstChild();

    while(!node.isNull())
    {
        element = node.toElement();

        if (element.tagName() == "Resources")
        {                        
            if (mGroupId.isEmpty() || element.attribute("id") == mGroupId)
            {
                resources->group << CreateResourceGroup(element);
            }            
        }

        node = node.nextSibling();
    }

    return resources.release();
}
