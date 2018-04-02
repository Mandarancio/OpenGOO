#pragma once

#include "wog_text.h"

template<>
struct AttributeReader<WOGText>
{
    void Read(const QDomAttr& aAttribute, WOGText* aOut)
    {
        auto name = aAttribute.name();
        if (name != QLatin1String("id"))
        {
            aOut->AddText(name, GetId(), aAttribute.value());
        }
    }

    void SetId(const QString& aId)
    {
        mId = aId;
    }

    const QString& GetId() const
    {
        return mId;
    }

private:
    QString mId;
};

