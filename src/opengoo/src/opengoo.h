#ifndef OPENGOO_H
#define OPENGOO_H

#include "og_gameengine.h"
#include "og_world.h"
#include "og_windowcamera.h"
#include "og_uiscene.h"
#include "og_event.h"

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
    float xSpeed;
    float ySpeed;
    float zoomSpeed;
    int pause;
}

struct Scroll
{
    bool up;
    bool down;
    bool left;
    bool right;
};

static const QString GAMEDIR = QDir::homePath() + "/.OpenGOO";
const QString MAIN_MENU = "MapWorldView";
const float K = 10.0f;
const int FRAMERATE = 60;
const int STEPS = 60;

OGGameEngine* _gameEngine = 0;
OGWorld* _world;

QTime* _gameTime = 0;
int _lastTime;
float _timeStep;
float _timeScrollStep;

QHash<QString, OGUI*> _listUI;

OGBall* _selectedBall = 0;

QString _levelname;
QString _island;

bool _isPause;
bool _E404;
bool _isScrollLock;
bool _isZoomLock;
bool _isMenu = false;

QList<OGEvent*> _listStates;

Scroll _scroll;
int _scrolltime = 0;
OGWindowCamera* _camera = 0;
bool _isMoveCamera;
int _width;
int _height;
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
    "en"      // language
};

void gooMessageHandler(QtMsgType, const QMessageLogContext &, const QString&);
void calculateFPS();
void closeGame();
void readConfiguration();

void mouseEven(OGUI* ui,QMouseEvent* e);

void createUI(const QString& name);
void createUIButtons();
void createUIBack();

void clearUI();

void createMenu(const QString &name);

void backToIsland();
void restartLevel();
void resumeGame();
void showOCDCriterial();

void loadLevel(const QString& name);
void reloadLevel();
void closeLevel();
QString getLevel(const QString& onclick);

QString getIsland();
void setIsland(const QString& name);
void loadIsland(const QString& name);

void loadMainMenu();

void initCamera();

void scroll();
void zoom(int direct);
void visualDebug(QPainter* painter, OGWorld* world, qreal zoom);

QPointF logicalToWindow(const QRectF & rect, qreal zoom);
QPoint windowToLogical(const QPoint & position);

void setBackgroundColor(const QColor & color);
void drawOpenGLScene();
void draw(QPainter* p, OGWorld* w);

// Animate camera
void updateCamera(int time);


#define UNIMPLEMENTED qWarning() << __FUNCTION__ << "is UNIMPLEMENTED!";

#endif // OPENGOO_H
