#pragma once

template<typename T>
struct AttributeReader;

template<typename T, typename AttributeReader>
inline void read_attributes(const QDomElement& aElement, T* aOut, AttributeReader& aAttributeReader)
{
    const auto attributes = aElement.attributes();
    for (int i = 0; i < attributes.size(); ++i)
    {
        auto item = attributes.item(i);
        aAttributeReader.Read(item.toAttr(), aOut);
    }
}
