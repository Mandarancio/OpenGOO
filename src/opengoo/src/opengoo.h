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
    bool visible;
};

struct Camera
{
    OGPosition position;
    qreal zoom;
};

struct Scroll
{
    bool up;
    bool down;
    bool left;
    bool right;
};

struct Button
{
    OGPosition position;
    QSize size;
    QString action;
};

typedef QList<OGSprite> OGSpriteList;

static const QString GAMEDIR = QDir::homePath() + "/.OpenGOO";

OGGameEngine* _gameEngine;
OGResources _resources;
OGStringList _strings;
OGSpriteList _resSprites;
QList<Button> _buttons;
bool _isLevelInitialize;
bool _isPause;
bool _E404;
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
void visualDebug(QPainter * painter);

#define UNIMPLEMENTED qWarning() << __FUNCTION__ << "is UNIMPLEMENTED!";

#endif // OPENGOO_H
