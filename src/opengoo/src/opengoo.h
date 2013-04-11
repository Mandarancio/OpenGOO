#ifndef OPENGOO_H
#define OPENGOO_H

#include "GameEngine/og_game.h"

#include <QColor>
#include <QList>
#include <QHash>
#include <QPoint>
#include <QString>

class OGBall;
class OGEvent;
class OGFPSCounter;
class OGWindowCamera;
class OGWorld;
class OGUI;

class QTime;

class OpenGOO : public OGGame
{
    OpenGOO() {}
    ~OpenGOO() {}

    static OpenGOO* pInstance_;

    OGWorld* pWorld_;
    OGFPSCounter* pFPS_;
    OGWindowCamera* pCamera_;
    OGBall* pSelectedBall_;

    QString levelName_;
    QString language_;
    QString island_;

    int width_;
    int height_;

    float timeStep_;
    float timeScrollStep_;

    QList<OGEvent*> eventList_;
    QHash<QString, OGUI*> uiList_;

    QTime* pGameTime_;
    int lastTime_;

    bool isPause_;

    QPoint lastMousePos_;
    QPoint curMousePos_;

    void _Start();
    void _End();

    void _Activate();
    void _Deactivate();

    void _Cycle();
    void _Paint(QPainter *painter);

    void _MouseButtonDown(QMouseEvent *ev);
    void _MouseButtonUp(QMouseEvent *ev);
    void _MouseMove(QMouseEvent *ev);

    void _Quit();

    void _CreateUI(const QString  &name);
    void _ClearUI();

    void _CreateUIBack();
    void _CloseUIBack();

    void _CreateUIButtons();

    void _LoadLevel(const QString& levelname);
    void _CloseLevel();
    void _ReloadLevel();

    void _SetDebug(bool debug);


    void _Scroll();
    void _SetBackgroundColor(const QColor &color);

    void _CreateMenu(const QString &name);

    QString _GetMainMenu();
    void _LoadMainMenu();

    QString _GetIsland();
    void _SetIsland(const QString &name);
    void _LoadIsland(const QString &name);

    // Handlers
    void _HQuit();
    void _HCreateMenu(OGEvent* ev);
    void _HRestart();
    void _HShowOCD();
    void _HBackToIsland();
    void _HResume();
    void _HBackToMainMenu();
    void _HLoadIsland(OGEvent* ev);
    void _HLoadLevel(OGEvent* ev);

public:
    static OpenGOO* instance();

    void Destroy();

    OGWorld* GetWorld() { return pWorld_; }
    OGBall* GetNearestBall();

    void SetLevelName(const QString& levelname);
    void SetLanguage(const QString& language);

    static void SendEvent(OGEvent* ev);

};

#endif // OPENGOO_H
