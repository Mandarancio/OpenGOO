#pragma once

#include <QFile>
#include <QDomDocument>

#include "Parsers/tagparsers.h"

struct DefaultTagParser
{
    typedef void Type;

    constexpr static const char* rootTag = "";

    void Parse(const QString&, const QDomElement&, Type*)
    {
    }
};

template<typename TTagParser>
class OGXmlConfig
{
public:
    OGXmlConfig()
    {
        SetRootTag(TTagParser::rootTag);
    }

    void Close();
    bool Read();

    bool IsOpen() const
    {
        return file_.isOpen();
    }

    bool Open(const QString& aFileName);

    bool Load(const QString& aFileName)
    {
        Close();
        return (Open(aFileName) && Read());
    }

    const QString GetFilename() const
    {
        return file_.fileName();
    }

    void SetRootTag(const QString& aTag)
    {
        rootTag_ = aTag;
    }

    void Parse(typename TTagParser::Type* aOut)
    {
        mTagParser.Parse(rootElement.tagName(), rootElement, aOut);
        parse_tags(rootElement, aOut,
                   [this](const QString& tag, const QDomElement& element, typename TTagParser::Type* out)
        {
            mTagParser.Parse(tag, element, out);
        });
    }

    std::unique_ptr<typename TTagParser::Type> Parse()
    {
        auto obj = TTagParser::Type::Create();
        Parse(obj.get());
        return obj;
    }

    ~OGXmlConfig()
    {
        Close();
    }

protected:
    bool OpenFile(const QString& filename);

protected:
    QDomElement rootElement;

private:
    QFile file_;
    QDomDocument domDoc_;
    QString rootTag_;
    TTagParser mTagParser;
};

#include <QFileInfo>
#include <QTextStream>

#include "decrypter.h"

template<typename TTagParser>
bool OGXmlConfig<TTagParser>::OpenFile(const QString& filename)
{
    file_.setFileName(filename);
    return file_.open(QIODevice::ReadOnly);
}

template<typename TTagParser>
bool OGXmlConfig<TTagParser>::Open(const QString& aFileName)
{
    if (QFile::exists(aFileName))
    {
        return OpenFile(aFileName);
    }

    if (QFile::exists(aFileName + ".xml"))
    {
        return OpenFile(aFileName + ".xml");
    }

    if (QFile::exists(aFileName + ".bin"))
    {
        return OpenFile(aFileName + ".bin");
    }

    return false;
}

template<typename TTagParser>
void OGXmlConfig<TTagParser>::Close()
{
    file_.close();
}

template<typename TTagParser>
bool OGXmlConfig<TTagParser>::Read()
{
    QFileInfo fi(file_.fileName());
    bool status = false;

    if (fi.suffix() == "bin")
    {
        QByteArray cryptedData = file_.readAll();
        auto xml_data = Decrypter().decrypt(cryptedData);
        status = domDoc_.setContent(xml_data);
#ifdef SAVE_BIN_TO_XML
        auto fn = file_.fileName() + QLatin1String(".xml");
        QFile file(fn);
        if (!file.exists() && file.open(QFile::WriteOnly | QFile::Text))
        {
            QTextStream ts(&file);
            ts << xml_data;
        }
#endif
    }
    else
    {
        status = domDoc_.setContent(&file_);
    }

    if (status)
    {
        rootElement = domDoc_.documentElement();

        if (rootElement.tagName() == rootTag_)
        {
            return true;
        }
    }

    return false;
}
