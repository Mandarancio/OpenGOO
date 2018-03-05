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
#include "GameEngine/camera.h"

#include "PhysicsEngine/og_physicsengine.h"

#include "og_fpscounter.h"

class OGWindowCamera;
class OGBall;
class EntityFactory;

class QTime;

typedef std::unique_ptr<EntityFactory> EntityFactoryPtr;

class OpenGOO : public og::OGGame
{
    Q_OBJECT

    friend class Level;
    friend class MainMenu;
    friend class Island;

public:
    static OpenGOO* GetInstance();

    void Destroy();

    void SetLanguage(const QString &language);

    void Quit()
    {
        _Quit();
    }

    bool isPause()
    {
        return mPause;
    }

    void SetPause(bool aPause)
    {
        mPause = aPause;
    }

    void GotoScene(const QString& a_scene)
    {
        m_gotoScene = a_scene;
    }

    EntityFactory& GetEntityFactory()
    {
        return *m_entityFactory;
    }

    og::Scene* GetScene()
    {
        return m_scene.get();
    }

    og::Camera* GetCamera()
    {
        return &mCamera;
    }

    int GetDeltaTime()
    {
        return m_deltaTime;
    }

private:
    OpenGOO();

    ~OpenGOO();

    static bool LevelIsExists(const QString& a_name);

    static QLatin1String GetMainMenuName();

    std::shared_ptr<og::Scene> CreateScene(const QString& a_name, bool aShouldCreatePhysics = true);

    std::shared_ptr<og::Scene> SetScene(std::shared_ptr<og::Scene> a_scene)
    {
        Q_ASSERT(a_scene && "a_scene is nullptr");
        m_scene->OnEnd();
        m_scene = a_scene;
        m_scene->OnBegin();

        return a_scene;
    }

    void LoadScene(const QString &a_name);

    void _Start();
    void _End();

    void _Activate();
    void _Deactivate();

    void _Cycle();
    void _Paint(QPainter* painter);

    void MouseButtonDown(const QPoint& aPos);
    void MouseButtonUp(const QPoint& aPos);
    void _MouseMove(QMouseEvent* ev);
    void _MouseWheel(QWheelEvent*)
    {
    }

    void _KeyDown(QKeyEvent* ev);
    void _KeyUp(QKeyEvent*)
    {
    }

    void _Quit();

private:
    static OpenGOO* pInstance_;

    static const char* types[];
    static const char* exts[];

    EntityFactoryPtr m_entityFactory;
    std::shared_ptr<og::Scene> m_scene;
    std::unique_ptr<OGFPSCounter> _pFPS;
    og::Camera mCamera;
    QPointF mCameraSpeed;

    QString m_language;
    int width_;
    int height_;

    QDateTime lastTime_;
    int m_deltaTime;

    bool mPause;

    QString m_gotoScene;
    QString m_previousScene;
};

#define GAME OpenGOO::GetInstance()
