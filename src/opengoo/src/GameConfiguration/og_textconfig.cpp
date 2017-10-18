#include "og_textconfig.h"
#include "wog_text.h"

struct OGTextConfigHelper
{
    static WOGString* CreateString(const QDomElement& element, const QString& language);
};

WOGString* OGTextConfigHelper::CreateString(const QDomElement& element, const QString& language)
{
    auto obj = std::unique_ptr<WOGString>(new WOGString);
    obj->id = element.attribute("id");
    obj->text = (language == "en") ? element.attribute("text") : element.attribute(language);

    return obj.release();
}

WOGText* OGTextConfig::Parser(const QString& language)
{
    WOGText* obj;
    QDomNode node;
    QDomElement element;

    obj = new WOGText;
    obj->language = language;

    node = rootElement.firstChild();

    while(!node.isNull())
    {
        element = node.toElement();

        if (element.tagName() == "string")
        {            
            obj->string << OGTextConfigHelper::CreateString(element, language);
        }

        node = node.nextSibling();
    }

    return obj;
}
