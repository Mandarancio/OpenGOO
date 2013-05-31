#ifndef OPENGOO_H
#define OPENGOO_H

#include "GameEngine/og_game.h"
#include "progresswindow.h"
#include <OGIPushButton>
#include "og_layer.h"
#include "island.h"
#include "level.h"
#include "og_fpscounter.h"

#include <QColor>
#include <QList>
#include <QHash>
#include <QMap>
#include <QPoint>
#include <QString>

#include <memory>


class OGWindowCamera;
class OGBall;
class OGWorld;

class QTime;

void visualDebug(QPainter* painter, OGWorld* world, qreal zoom);

class OpenGOO : public og::OGGame
{
    Q_OBJECT

    Q_DISABLE_COPY(OpenGOO)

    public:
        static OpenGOO* instance();

        void Destroy();

        OGWorld* GetWorld() { return pWorld_; }
        OGBall* GetNearestBall();

        void SetLevelName(const QString &levelname);
        void SetLanguage(const QString &language);                

        void OpenPipe();
        void ClosePipe();

        void ShowProgress();

        void Quit() { _Quit(); }

        void LoadIsland(const QString &name);

        void AddSprite(float depth, OGSprite* sprite);
        void ClearSprites();

        void ReloadLevel();

        bool isPause() { return _pause; }
        void SetPause(bool pause) { _pause = pause; }

        friend class Level;
        friend class MainMenu;
        friend class Island;

        void loadLevel(const QString &levelname);

    private:
        OpenGOO() {}
        ~OpenGOO() {}

        static OpenGOO* pInstance_;

        OGWorld* pWorld_;
        std::unique_ptr<OGFPSCounter> _pFPS;
        OGWindowCamera* pCamera_;

        OGBall* _pSelectedBall;
        void _ClearSelectedBall() { _pSelectedBall = 0; }

        QString levelName_;
        QString language_;
        QString _currentIsland;

        int width_;
        int height_;

        float timeStep_;
        float timeScrollStep_;        

        QTime* pGameTime_;
        int lastTime_;

        bool _pause;

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

        void _KeyDown(QKeyEvent* ev);
        void _KeyUp(QKeyEvent* ev) { Q_UNUSED(ev)}

        // Layers
        QMap<float, OGLayer> layers_;

        void _ClearLayers();

        void _Quit();

        void _SetDebug(bool debug);

        void _Scroll();
        void _SetBackgroundColor(const QColor &color);        

        // Main menu
        QString _GetMainMenu();
        void _LoadMainMenu();
        void _CloseMainMenu();

        // Island
        std::unique_ptr<Island> pIsland;
        QString _GetIsland();
        void _SetIsland(const QString &name);
        void _CreateIsland(const QString &name);
        void _RemoveIsland();

        // Level
        std::unique_ptr<Level> pLevel_;
        void _CreateLevel(const QString &levelname);
        void _LoadLevel(const QString &levelname);
        void _RemoveLevel();
        void _CloseLevel();        

        std::unique_ptr<ProgressWindow> pProgressWnd_;
        void _InitProgressWindow();
        void _SaveProgress();

        std::unique_ptr<og::ui::IPushButton> pContinueBtn_;
        void _CreateContinueButton();

private slots:
        void _backToMainMenu();
        void _backToIsland();
        void _closeContinueButton();
        void _closeProgressWindow();
};

#endif // OPENGOO_H
