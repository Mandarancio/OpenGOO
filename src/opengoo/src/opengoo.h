#ifndef OPENGOO_H
#define OPENGOO_H

#include <og_gameengine.h>

#include <QDir>

#include "og_gameconfig.h"
#include "og_resourceconfig.h"
#include "og_videomode.h"
#include "og_textconfig.h"
#include "og_sceneconfig.h"
#include "og_levelconfig.h"

struct OGSprite
{
    QPointF pos;
    QPointF scale;
    qreal rotation;
    QPixmap sprite;
    qreal depth;
    QColor color;
    qreal opacity;
};

static const QString GAMEDIR = QDir::homePath() + "/.OpenGOO";

OGGameEngine* _gameEngine;
QList <OGResource> _resources;
QList <OGText> _strings;
QList <OGSprite> _resSprites;
bool _isMainMenu;
bool _isMainMenuInitialize;
bool _isVideoModeSupported;
QRectF _camera;
OGVideoMode _vm;
OGScene _scene;
OGLevel _level;



void gooMessageHandler(QtMsgType, const QMessageLogContext &, const QString&);
void mainMenu(QPainter* painter);
QString gGetResource(OGResource::Type type, const QString & id);
void createSprite(OGSprite* sprite, const QString & image);

#endif // OPENGOO_H
