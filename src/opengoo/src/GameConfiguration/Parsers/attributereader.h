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

template<typename T>
inline T& AddElement(const QDomElement& aElement, std::list<T>& aList)
{
    aList.push_back(T());
    auto& el = aList.back();
    AttributeReader<T> reader;
    read_attributes(aElement, &el, reader);
    return el;
}

template<typename T>
inline void ReadAttribute(const QDomAttr& aAttribute, T* aOut)
{
    AttributeReader<T>::Read(aAttribute, aOut);
}
