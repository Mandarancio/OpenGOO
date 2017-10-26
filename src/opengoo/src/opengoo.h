#pragma once

#include <memory>

#include <QColor>
#include <QDateTime>
#include <QList>
#include <QHash>
#include <QMap>
#include <QPoint>
#include <QString>

#include "GameEngine/og_game.h"
#include "GameEngine/scene.h"

#include "progresswindow.h"
#include <OGIPushButton>
#include "og_layer.h"
#include "island.h"
#include "level.h"
#include "og_fpscounter.h"
#include "og_world.h"
#include "camera.h"

class OGWindowCamera;
class OGBall;
class EntityFactory;

class QTime;

void visualDebug(QPainter* painter, OGWorld* world, qreal zoom);

typedef std::unique_ptr<EntityFactory> EntityFactoryPtr;

class OpenGOO : public og::OGGame
{
    Q_OBJECT

    Q_DISABLE_COPY(OpenGOO)

    static const char* types[];
    static const char* exts[];

    public:
        static OpenGOO* GetInstance();

        void Destroy();

        OGWorld* GetWorld()
        {
            return static_cast<OGWorld*>(GetScene());
        }

        OGBall* GetNearestBall();

        void SetLanguage(const QString &language);

        void OpenPipe();
        void ClosePipe();

        void ShowProgress();

        void Quit()
        {
            _Quit();
        }

        void LoadIsland(const QString &name);

        void AddSprite(float depth, OGSprite* sprite);
        void AddSprite(OGSprite* sprite);
        void ClearSprites();

        void ReloadLevel();

        bool isPause()
        {
            return mPause;
        }

        void SetPause(bool aPause)
        {
            mPause = aPause;
        }

        friend class Level;
        friend class MainMenu;
        friend class Island;

        void loadLevel(const QString &levelname);

        void GotoScene(const QString& a_scene)
        {
            m_gotoScene = a_scene;
        }

        EntityFactory& GetEntityFactory() const
        {
            return *m_entityFactory;
        }

        og::Scene* GetScene() const
        {
            return m_scene.get();
        }

        Camera& GetCamera()
        {
            return mCamera;
        }

        const Camera& GetCamera() const
        {
            return mCamera;
        }

        int GetDeltaTime() const
        {
            return m_deltaTime;
        }

        QPoint WindowToLogical(const QPoint& p);

    static void SetDebug(OGWorld&, bool);

    private:
        OpenGOO();

        ~OpenGOO();

        static bool LevelIsExists(const QString& a_name);

    private:
        static OpenGOO* pInstance_;

        EntityFactoryPtr m_entityFactory;
        std::shared_ptr<og::Scene> m_scene;
        std::unique_ptr<OGFPSCounter> _pFPS;
        Camera mCamera;

        bool mSceneIsLoaded;

    private:
        std::shared_ptr<og::Scene> CreateScene();

        std::shared_ptr<og::Scene> SetScene(std::shared_ptr<og::Scene> a_scene)
        {
            assert(a_scene && "a_scene is nullptr");

            m_scene = a_scene;

            return a_scene;
        }

    private:
        QString m_language;
        QString _currentIsland;

        int width_;
        int height_;

        float timeStep_;
        float timeScrollStep_;

        QDateTime lastTime_;
        int m_deltaTime;

        bool mPause;

        bool isContinue_;
        bool isLevelExit_;

        QPoint lastMousePos_;
        QPoint curMousePos_;

        int balls_;
        int ballsRequired_;

    private:
        void _Start();
        void _End();

        void _Activate();
        void _Deactivate();

        void _Cycle();
        void _Paint(QPainter* painter);

        void _MouseButtonDown(QMouseEvent*);
        void _MouseButtonUp(QMouseEvent*);
        void _MouseMove(QMouseEvent* ev);
        void _MouseWheel(QWheelEvent*)
        {
        }

        void _KeyDown(QKeyEvent* ev);
        void _KeyUp(QKeyEvent*)
        {
        }

        void LoadScene(og::Scene* a_scene);

    private:
        // Layers
        QMap<float, OGLayer> layers_;

    private:
        void _ClearLayers();

        void _Quit();

        void Scroll();
        void _SetBackgroundColor(const QColor &color);

        // Main menu
        QString _GetMainMenu();
        void _LoadMainMenu();
        void _CloseMainMenu();

        // Island
    private:
        std::unique_ptr<Island> pIsland;

    private:
        QString _GetIsland() const
        {
            return _currentIsland;
        }

        void _SetIsland(const QString &name)
        {
             _currentIsland = name;
        }

        void _CreateIsland(const QString &name);
        void _RemoveIsland();

        // Level
    private:
        std::unique_ptr<Level> pLevel_;

    private:
        void _CreateLevel(const QString &levelname);
        bool _LoadLevel(OGWorld* a_world, const QString&);
        void _RemoveLevel();
        void _CloseLevel();

   private:
        std::unique_ptr<ProgressWindow> pProgressWnd_;

   private:
        void _InitProgressWindow();
        void _SaveProgress();

   private:
        std::unique_ptr<og::ui::IPushButton> pContinueBtn_;

   private:
        void _CreateContinueButton();

private slots:
        void _backToMainMenu();
        void _backToIsland();
        void _closeContinueButton();
        void _closeProgressWindow();

private:
        QString m_gotoScene;
        QString m_previousScene;
};

#define GAME OpenGOO::GetInstance()
