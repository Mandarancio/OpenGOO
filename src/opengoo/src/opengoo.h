#ifndef OPENGOO_H
#define OPENGOO_H

#include <og_gameengine.h>

#include <QDir>

#include <OGConfiguration>

typedef QList<OGButton> OGButtonList;

enum OGAction {UP, OVER, DISABLED};

struct OGSprite
{
    OGPosition position;
    OGScale scale;
    qreal rotation;
    QPixmap sprite;
    qreal depth;
    QColor color;
    qreal opacity;
    QSize size;
};

struct Camera
{
    OGPosition position;
    qreal zoom;
    OGPosition logPosition;
};

struct Scroll
{
    bool up;
    bool down;
    bool left;
    bool right;
};

typedef QList<OGSprite> OGSpriteList;

static const QString GAMEDIR = QDir::homePath() + "/.OpenGOO";

OGGameEngine* _gameEngine;
OGResources _resources;
OGStringList _strings;
OGSpriteList _resSprites;
OGButtonList _buttons;
bool _isLevelInitialize;
Camera _camera;
OGScene _scene;
OGLevel _level;
QString _levelname;
Scroll _scroll;

// Default settings
OGConfig _config = {
    800,    // width
    600,    // height
    true,   // fullscreen
    ""      // language
};

void gooMessageHandler(QtMsgType, const QMessageLogContext &, const QString&);
void createScene(const QString & levelname);
void createSprite(OGSprite* sprite, const QString & image);
void readGameConfiguration(const QString & level);
void createButton(const OGButton & button, OGAction action);
void scroll();
void zoom(int direct);

#define UNIMPLEMENTED qWarning() << __FUNCTION__ << "is UNIMPLEMENTED!";

#endif // OPENGOO_H
