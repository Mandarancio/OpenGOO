#ifndef OPENGOO_H
#define OPENGOO_H

#include "og_gameengine.h"
#include "physics.h"
#include "og_world.h"
#include "og_windowcamera.h"

#include <QDir>
#include <QTime>
#include <QTimer>

namespace physics
{
    extern const float K = 0.1f;
    extern bool status;
}

namespace camera
{
    bool isPause;
    bool isInitialization = true;
    int nextCamera = 1;
    int numberCameras = 0;
    int traveltime;
    int sumTime;
    qreal xSpeed;
    qreal ySpeed;
    qreal zoomSpeed;
    qreal pause;
}

struct Scroll
{
    bool up;
    bool down;
    bool left;
    bool right;
};

static const QString GAMEDIR = QDir::homePath() + "/.OpenGOO";
const qreal K = 10.0;
const int FRAMERATE = 60;
const int STEPS = 60;

OGGameEngine* _gameEngine = 0;
OGPhysicsEngine* _physicsEngine = 0;
OGWorld* _world;

QTime* _gameTime = 0;
int _lastTime;
qreal _timeStep;
qreal _timeScrollStep;

QList<OGSprite*>* _sprites;
QList<OGButton*>* _buttons = 0;

OGBall* _selectedBall = 0;

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
QTimer _timer;
int _nearestCounter = 0;
int _fps = FRAMERATE;
int _cur_fps = 0;
QPoint _lastMousePos;

// Default settings
OGConfig _config = {
    800,    // width
    600,    // height
    true,   // fullscreen
    ""      // language
};

void gooMessageHandler(QtMsgType, const QMessageLogContext &, const QString&);
void calculateFPS();
void closeGame();
void readConfiguration();
void scroll();
void zoom(int direct);
void visualDebug(QPainter* painter, OGWorld* world, qreal zoom);
void buttonMenuAction();
void buttonMenu();
void Event(const QString & event);

QPointF logicalToWindow(const QRectF & rect, qreal zoom);
QPoint windowToLogical(const QPoint & position);

void setBackgroundColor(const QColor & color);
void drawOpenGLScene();

// Animate camera
void updateCamera(int time);


#define UNIMPLEMENTED qWarning() << __FUNCTION__ << "is UNIMPLEMENTED!";

#endif // OPENGOO_H
