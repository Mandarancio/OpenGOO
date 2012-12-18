#ifndef OPENGOO_H
#define OPENGOO_H

#include <og_gameengine.h>

#include <QDir>
#include <QXmlStreamWriter>

struct OGGameConfig
{
    int screen_width;
    int screen_height;
    QString language;
    bool fullscreen;
};

static const QString GAMEDIR = QDir::homePath() + "/.OpenGOO";

OGGameEngine* _gameEngine;

void gooMessageHandler(QtMsgType, const QMessageLogContext &, const QString&);
void readConfig(OGGameConfig* config);

#endif // OPENGOO_H
