#include "og_resourceconfig.h"

OGResourceConfig::OGResourceConfig(const QString & filename)
    : OGXmlConfig(filename)
{
    SetRootTag("ResourceManifest");
}

WOGResources* OGResourceConfig::Parser(QString groupid)
{
    WOGResources* resources;
    QDomNode node;
    QDomElement element;

    defaultPath_ = "./";
    defaultIdPrefix_ = "";
    resources = new WOGResources;
    node = rootElement.firstChild();

    while(!node.isNull())
    {
        element = node.toElement();

        if (element.tagName() == "Resources")
        {                        
            if (groupid.isEmpty() || element.attribute("id") == groupid)
            {
                resources->group << CreateResourceGroup(element);
            }            
        }

        node = node.nextSibling();
    }

    return resources;
}

WOGResource* OGResourceConfig::CreateResource(const QDomElement & element
                                              , WOGResource::Type type
                                              )
{
    WOGResource* obj;

    obj = new WOGResource;
    obj->type = type;
    obj->id = defaultIdPrefix_ + element.attribute("id");
    obj->path = defaultPath_ + element.attribute("path");

    return obj;
}

WOGResourceGroup* OGResourceConfig::CreateResourceGroup(
        const QDomElement & element
        )
{
    WOGResourceGroup*  obj;
    QDomNode node;
    QDomElement resElement;

    obj = new WOGResourceGroup;
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
            obj->resource << CreateResource(resElement
                                             , WOGResource::IMAGE);
        }
        else if (resElement.tagName() == "Sound")
        {
            obj->resource << CreateResource(resElement
                                             , WOGResource::SOUND);
        }
        else if (resElement.tagName() == "font")
        {
            obj->resource << CreateResource(resElement
                                             , WOGResource::FONT);
        }

        node = node.nextSibling();
    }

    return obj;
}
