#ifndef OPENGOO_H
#define OPENGOO_H

#include "GameEngine/og_game.h"
#include "progresswindow.h"
#include "og_uipushbutton.h"

#include <QColor>
#include <QList>
#include <QHash>
#include <QPoint>
#include <QString>

#include <memory>

class OGEvent;
class OGFPSCounter;
class OGWindowCamera;
class OGUI;
class OGBall;
class OGWorld;

class QTime;

void visualDebug(QPainter* painter, OGWorld* world, qreal zoom);

class OpenGOO : public OGGame
{        
    public:
        static OpenGOO* instance();

        void Destroy();

        OGWorld* GetWorld() { return pWorld_; }
        OGBall* GetNearestBall();

        void SetLevelName(const QString &levelname);
        void SetLanguage(const QString &language);

        static void SendEvent(OGEvent* ev);

        void OpenPipe();
        void ClosePipe();

        void CreateUI(const QString  &name) { _CreateUI(name); }

        void ShowProgress();

        void Quit() { _Quit(); }

private:
        OpenGOO() {}
        ~OpenGOO() {}

        OpenGOO(const OpenGOO&);
        OpenGOO& operator= (const OpenGOO&);

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
        bool isContinue_;
        bool isLevelExit_;

        QPoint lastMousePos_;
        QPoint curMousePos_;

        int balls_;
        int ballsRequired_;

        void _Start();
        void _End();

        void _Activate();
        void _Deactivate();

        void _Cycle();
        void _Paint(QPainter* painter);

        void _MouseButtonDown(QMouseEvent* ev);
        void _MouseButtonUp(QMouseEvent* ev);
        void _MouseMove(QMouseEvent* ev);
        void _MouseWheel(QWheelEvent* ev) { Q_UNUSED(ev)}

        void _KeyDown(QKeyEvent* ev) { Q_UNUSED(ev)}
        void _KeyUp(QKeyEvent* ev) { Q_UNUSED(ev)}

        void _Quit();

        void _CreateUI(const QString  &name);
        void _ClearUI();

        void _CreateUIBack();
        void _CloseUIBack();

        void _CreateUIButtons();

        void _LoadLevel(const QString &levelname);
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

        std::unique_ptr<ProgressWindow> pProgressWnd_;
        void _InitProgressWindow();
        void _SaveProgress();

        std::unique_ptr<OGUIPushButton> pContinueBtn_;
        std::unique_ptr<OGUIPushButton> _CreateContinueButton();

        // Handlers
        void _HCreateMenu(OGEvent* ev);
        void _HRestart();
        void _HShowOCD();
        void _HBackToIsland();
        void _HResume();
        void _HBackToMainMenu();
        void _HLoadIsland(OGEvent* ev);
        void _HLoadLevel(OGEvent* ev);

};

#endif // OPENGOO_H
