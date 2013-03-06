#include "og_textconfig.h"

OGTextConfig::OGTextConfig(const QString & filename)
    :OGXmlConfig(filename)
{
    SetRootTag("strings");
}

WOGText* OGTextConfig::Parser(const QString & language)
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
            obj->string << CreateString(element, language);
        }

        node = node.nextSibling();
    }

    return obj;
}

WOGString* OGTextConfig::CreateString(const QDomElement & element
                                      , const QString & language
                                      )
{
    WOGString* obj;

    obj = new WOGString;
    obj->id = element.attribute("id");

    if (language == "en") { obj->text = element.attribute("text"); }
    else if (language == "es") { obj->text = element.attribute("es"); }
    else if (language == "fr") { obj->text = element.attribute("es"); }
    else if (language == "de") { obj->text = element.attribute("es"); }
    else if (language == "it") { obj->text = element.attribute("es"); }
    else if (language == "ru") { obj->text = element.attribute("es"); }

    return obj;
}
