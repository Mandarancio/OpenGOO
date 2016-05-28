#include "og_gameconfig.h"
#include <QTextStream>
#include <QXmlStreamWriter>

OGGameConfig::OGGameConfig(const QString & filename)
    :OGXmlConfig(filename)
{
    SetRootTag("config");
}

OGConfig OGGameConfig::Parser()
{
    OGConfig config;
    QDomNode node = rootElement.firstChild();

    while (!node.isNull())
    {
        QDomElement domElement = node.toElement();

        if (domElement.tagName() == "param")
        {
            QString attribute = domElement.attribute("name", "");

            if (attribute == "screen_width")
            {
                config.screen_width = domElement.attribute("value").toInt();
            }
            else if (attribute == "screen_height")
            {
                config.screen_height = domElement.attribute("value").toInt();
            }
            else if (attribute == "language")
            {
                config.language = domElement.attribute("value");
            }
            else if (attribute == "fullscreen")
            {
                if (domElement.attribute("value") == "true")
                {
                    config.fullscreen = true;
                }
                else { config.fullscreen = false; }
            }
            else if (attribute == "refreshrate")
            {
                config.refreshrate = domElement.attribute("value").toInt();
            }
        }

        node = node.nextSibling();
    }

    return config;
}

void OGGameConfig::Create(OGConfig &config)
{
    QFile file(GetFilename());
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
    stream.writeAttribute("value",  QString::number(config.screen_width));
    stream.writeEndElement(); // end screen_width

    stream.writeStartElement("param");
    stream.writeAttribute("name", "screen_height");
    stream.writeAttribute("value",  QString::number(config.screen_height));
    stream.writeEndElement(); // end screen_height

    stream.writeStartElement("param");
    stream.writeAttribute("name", "fullscreen");

    if (config.fullscreen) { stream.writeAttribute("value", "true"); }
    else { stream.writeAttribute("value",  "false"); }

    stream.writeEndElement(); // end fullscreen

    stream.writeEndElement(); // end config

    stream.writeEndDocument();
    out << xmlData;
    file.close();
}
