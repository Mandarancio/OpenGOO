#ifndef OPENGOO_H
#define OPENGOO_H

#include "og_gameengine.h"
#include "physics.h"
#include "og_world.h"
#include "og_windowcamera.h"

#include <QDir>
#include <QTime>

namespace physics
{
    extern const qreal K = 0.1;
    extern bool status;
}

struct Scroll
{
    bool up;
    bool down;
    bool left;
    bool right;
};

struct OGBall
{
    OGPhysicsBody* ball;
    QString id;
    bool active;
    bool selected;

    OGBall() : ball(0), id(QString()), active(false), selected(false) {}
    ~OGBall() { delete ball; }
};

struct OGStrand
{
    int gb1;
    int gb2;
    OGPhysicsJoint* strand;

    OGStrand() : gb1(-1), gb2(-1), strand(0) { }
};

static const QString GAMEDIR = QDir::homePath() + "/.OpenGOO";

OGGameEngine* _gameEngine = 0;
OGPhysicsEngine* _physicsEngine = 0;
OGWorld* _world;

QList<OGSprite*>* _sprites;
QList<OGButton*>* _buttons = 0;
QList<OGPhysicsBody*> _staticCircles;
QList<OGPhysicsBody*> _staticLines;
QList<OGPhysicsBody*> _staticRectangles;
QList<OGBall*> _balls;
int _selectedBall = -1;
QList<OGStrand*> _strands;
OGButton _buttonMenu;

QString _levelname;
bool _isLevelInitialize;
bool _isPause;
bool _E404;
bool _isScrollLock;
bool _isZoomLock;
Scroll _scroll;
int _scrolltime = 0;
OGWindowCamera _camera;
bool _isMoveCamera;
qreal _width;
qreal _height;
QTime _time;

// Default settings
OGConfig _config = {
    800,    // width
    600,    // height
    true,   // fullscreen
    ""      // language
};

void gooMessageHandler(QtMsgType, const QMessageLogContext &, const QString&);
void closeGame();
void readConfiguration();
void scroll();
void zoom(int direct);
void visualDebug(QPainter* painter, OGWorld* world, qreal zoom);
void buttonMenuAction();
void buttonMenu();

QPointF logicalToWindow(const QRectF & rect, qreal zoom);
QPoint windowToLogical(const QPoint & position);

OGBall* createBall(WOGBallInstance* data, WOGBall* configuration);
OGStrand* createStrand(WOGStrand* strand);

bool createPhysicsWorld();
void clearPhysicsWorld();

void setBackgroundColor(const QColor & color);
void drawOpenGLScene();

// Animate camera
int _nextCamera = 1;
int _numberCameras = 0;
qreal _dx = 0;
qreal _dy = 0;
qreal _dzoom = 0;
qreal _pause = 0;
qreal _frames = 0;
void moveCamera();

WOGBall* readBallConfiguration(const QString & dirname);

#define UNIMPLEMENTED qWarning() << __FUNCTION__ << "is UNIMPLEMENTED!";

#endif // OPENGOO_H
