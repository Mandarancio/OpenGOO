#ifndef OPENGOO_H
#define OPENGOO_H

#include <og_gameengine.h>

#include <QDir>

#include <OGConfiguration>

struct OGSprite
{
    OGPosition pos;
    OGScale scale;
    qreal rotation;
    QPixmap sprite;
    qreal depth;
    QColor color;
    qreal opacity;
};

static const QString GAMEDIR = QDir::homePath() + "/.OpenGOO";

OGGameEngine* _gameEngine;
OGResources _resources;
OGStringList _strings;
QList <OGSprite> _resSprites;
bool _isMainMenu;
bool _isMainMenuInitialize;
QRectF _camera;
OGScene _scene;
OGLevel _level;

// Default settings
OGConfig _config = {
    800,    // width
    600,    // height
    true,   // fullscreen
    ""      // language
};

void gooMessageHandler(QtMsgType, const QMessageLogContext &, const QString&);
void mainMenu(QPainter* painter);
void createSprite(OGSprite* sprite, const QString & image);
void readGameConfiguration();

#define UNIMPLEMENTED qWarning() << __FUNCTION__ << "is UNIMPLEMENTED!";

#endif // OPENGOO_H
