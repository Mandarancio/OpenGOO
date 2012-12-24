#ifndef OPENGOO_H
#define OPENGOO_H

#include <og_gameengine.h>

#include <QDir>

#include "og_gameconfig.h"
#include "og_resourceconfig.h"
#include "og_videomode.h"
#include "og_textconfig.h"
#include "og_sceneconfig.h"

static const QString GAMEDIR = QDir::homePath() + "/.OpenGOO";

OGGameEngine* _gameEngine;
QList <OGResource> _resources;
QList <OGText> _strings;
QList <QPixmap> _resImages;
bool _isMainMenu;
bool _isMainMenuInitialize;
bool _isVideoModeSupported;
QRectF _camera;
OGVideoMode _vm;
OGScene _scene;



void gooMessageHandler(QtMsgType, const QMessageLogContext &, const QString&);
void mainMenu(QPainter* painter);
QString GetResource(OGResource::Type type, QString id);

#endif // OPENGOO_H
