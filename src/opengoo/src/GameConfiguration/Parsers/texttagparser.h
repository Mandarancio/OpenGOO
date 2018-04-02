#pragma once

#include "textattributereader.h"

template<>
struct TagParser<WOGText>
{
    typedef WOGText Type;

    constexpr static const char* rootTag = "strings";

    void Parse(const QString& aTag, const QDomElement& aElement, Type* aOut)
    {
        if (aTag == QLatin1String("string"))
        {
            auto id = aElement.attribute(QLatin1String("id"));
            mAttributeReader.SetId(id);
            const auto attributes = aElement.attributes();
            for (int i = 0; i < attributes.size(); ++i)
            {
                auto item = attributes.item(i);
                mAttributeReader.Read(item.toAttr(), aOut);
            }
        }
    }

private:
    AttributeReader<Type> mAttributeReader;
};
