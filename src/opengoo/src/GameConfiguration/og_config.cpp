#include <QTextStream>
#include <QXmlStreamWriter>
#include <QFile>

#include "og_xmlconfig.h"

class ParamWriter
{
public:
    ParamWriter(QXmlStreamWriter& aStream)
        : mStream(aStream)
    {
    }

    void Write(const QString& aName, const QString& aValue)
    {
        mStream.writeStartElement("param");
        mStream.writeAttribute("name",  aName);
        mStream.writeAttribute("value", aValue);
        mStream.writeEndElement();
    }

    void Write(const QString& aName, int aValue)
    {
        Write(aName, QString::number(aValue));
    }

    void Write(const QString& aName, float aValue)
    {
        Write(aName, QString::number(aValue));
    }

    void Write(const QString& aName, bool aValue)
    {
        Write(aName, aValue ? QStringLiteral("true") : QStringLiteral("false"));
    }

private:
    QXmlStreamWriter& mStream;
};

bool OGConfig::Load(const QString& aFileName)
{
    OGXmlConfig<TagParser<OGConfig>> config;
    if (!config.Load(aFileName))
    {
        return false;
    }

    config.Parse(this);
    return true;
}

void OGConfig::Write(const QString& aFileName)
{
    QFile file(aFileName);
    file.open(QIODevice::WriteOnly);
    QString xmlData;
    QTextStream out(&file);
    QXmlStreamWriter stream(&xmlData);
    stream.setAutoFormatting(true);
    stream.writeStartDocument();
    stream.writeStartElement("config");

    ParamWriter pwriter(stream);
    pwriter.Write("language", language);
    pwriter.Write("screen_width", screen_width);
    pwriter.Write("screen_height", screen_height);
    pwriter.Write("fullscreen", fullscreen);
    pwriter.Write("ui_scale", uiScale);

    stream.writeEndElement(); // end config

    stream.writeEndDocument();
    out << xmlData;
    file.close();
}
