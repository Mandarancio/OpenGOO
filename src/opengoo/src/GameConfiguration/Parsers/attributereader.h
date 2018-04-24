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
inline void read_attributes(const QDomElement& aElement, T* aOut)
{
    const auto attributes = aElement.attributes();
    for (int i = 0; i < attributes.size(); ++i)
    {
        auto item = attributes.item(i);
        AttributeReader<T>::Read(item.toAttr(), aOut);
    }
}

template<typename T>
inline typename T::value_type& AddElement(const QDomElement& aElement, T& aList)
{
    typedef typename T::value_type type;
    aList.push_back(type());
    auto& el = aList.back();
    AttributeReader<type> reader;
    read_attributes(aElement, &el, reader);
    return el;
}

template<typename T>
inline void ReadAttribute(const QDomAttr& aAttribute, T* aOut)
{
    AttributeReader<T>::Read(aAttribute, aOut);
}
