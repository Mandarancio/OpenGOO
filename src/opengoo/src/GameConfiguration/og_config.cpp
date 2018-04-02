#include <QTextStream>
#include <QXmlStreamWriter>
#include <QFile>

#include "og_xmlconfig.h"

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

    stream.writeStartElement("param");
    stream.writeAttribute("name", "language");
    stream.writeAttribute("value",  "en");
    stream.writeEndElement(); // end language

    stream.writeStartElement("param");
    stream.writeAttribute("name", "screen_width");
    stream.writeAttribute("value",  QString::number(screen_width));
    stream.writeEndElement(); // end screen_width

    stream.writeStartElement("param");
    stream.writeAttribute("name", "screen_height");
    stream.writeAttribute("value",  QString::number(screen_height));
    stream.writeEndElement(); // end screen_height

    stream.writeStartElement("param");
    stream.writeAttribute("name", "fullscreen");

    if (fullscreen)
    {
        stream.writeAttribute("value", "true");
    }
    else
    {
        stream.writeAttribute("value",  "false");
    }

    stream.writeEndElement(); // end fullscreen

    stream.writeEndElement(); // end config

    stream.writeEndDocument();
    out << xmlData;
    file.close();
}
