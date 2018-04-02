#pragma once

template<typename T>
struct TagParser;

template<typename T, typename Parser>
inline void parse_tags(const QDomElement& aElement, T* aOut, Parser aParser)
{
    for (auto node = aElement.firstChild(); !node.isNull(); node = node.nextSibling())
    {
        if (!node.isElement())
        {
            continue;
        }

        auto element = node.toElement();
        const auto tag = element.tagName();
        aParser(tag, element, aOut);
    }
}
