#pragma once

#include <QString>
#include <QHash>

struct WOGText
{
    typedef QHash<QString, QString> TextMap;

    QHash<QString, TextMap> string;

    void AddText(const QString& aLanguage, const QString& aId, QString&& aText)
    {
        string[aLanguage][aId] = aText;
    }

    bool HasLanguage(const QString& aLanguage) const
    {
        return string.contains(aLanguage);
    }

    TextMap& GetTextMap(const QString& aLanguage)
    {
        if (!HasLanguage(aLanguage))
        {
            throw std::out_of_range("Language not found");
        }

        return string[aLanguage];
    }

    static std::unique_ptr<WOGText> Create()
    {
        return std::unique_ptr<WOGText>(new WOGText);
    }
};
