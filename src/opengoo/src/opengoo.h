#ifndef OPENGOO_H
#define OPENGOO_H

#include "og_gameengine.h"
#include "og_world.h"
#include "og_windowcamera.h"
#include "og_uiscene.h"
#include "og_event.h"
#include "OGLib/icamera.h"

#include <QDir>
#include <QTime>
#include <QTimer>

#include <memory>

namespace FPSCounter
{
    int sumFPSTime = 0;
}

const QString GAMEDIR = QDir::homePath() + "/.OpenGOO";
const QString MAIN_MENU = "MapWorldView";
const float K = 10.0f;
const int FRAMERATE = 60;
const int STEPS = 60;

OGGameEngine* _gameEngine = 0;
OGWorld* _world;

std::unique_ptr<QTime> _gameTime;
int _lastTime;
float _timeStep;
float _timeScrollStep;

QHash<QString, OGUI*> _listUI;

OGBall* _selectedBall = 0;

QString _levelname;
QString _island;

bool _isPause;

QList<OGEvent*> _listStates;

int _scrolltime = 0;
std::auto_ptr<OGWindowCamera> _camera;
int _width;
int _height;
int _fps = FRAMERATE;
int _cur_fps = 0;
QPoint _lastMousePos;
QPoint _curMousePos;

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

QString getIsland();
void setIsland(const QString& name);
void loadIsland(const QString& name);

void loadMainMenu();

void scroll();
void visualDebug(QPainter* painter, OGWorld* world, qreal zoom);

QPointF logicalToWindow(const QRectF & rect, qreal zoom);

void setBackgroundColor(const QColor & color);
void drawOpenGLScene();
void draw(QPainter* p, OGWorld* w);


void setDebug(bool debug);

#define UNIMPLEMENTED qWarning() << __FUNCTION__ << "is UNIMPLEMENTED!";

#endif // OPENGOO_H
