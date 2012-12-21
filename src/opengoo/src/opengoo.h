#ifndef OPENGOO_H
#define OPENGOO_H

#include <og_gameengine.h>

#include <QDir>

#include "og_gameconfig.h"
#include "og_resourceconfig.h"

static const QString GAMEDIR = QDir::homePath() + "/.OpenGOO";

OGGameEngine* _gameEngine;
QList <OGResource> _resources;
bool _isMainMenu;

void gooMessageHandler(QtMsgType, const QMessageLogContext &, const QString&);
void mainMenu(QPainter* painter);

#endif // OPENGOO_H
