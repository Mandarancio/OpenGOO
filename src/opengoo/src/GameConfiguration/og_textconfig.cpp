#include "og_textconfig.h"
#include "wog_text.h"

OGTextConfig::Type OGTextConfig::Parser()
{
    QString attr = (mLanguage == "en" || mLanguage.isEmpty()) ? "text" : mLanguage;
    auto obj = Type(new WOGText);
    obj->language = mLanguage;

    for (auto node = rootElement.firstChild(); !node.isNull(); node = node.nextSibling())
    {
        auto element = node.toElement();
        if (element.tagName() == "string")
        {
            obj->string[element.attribute("id")] = element.attribute(attr);
        }
    }

    return obj;
}
