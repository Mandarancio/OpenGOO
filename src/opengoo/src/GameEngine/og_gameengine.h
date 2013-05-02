#ifndef OG_GAMEENGINE_H
#define OG_GAMEENGINE_H

#include <QObject>

#include "og_window.h"

class OGEvent;
class OGGame;
class OGPhysicsEngine;
class OGResourceManager;

class OGGameEngine : public QObject
{
        Q_OBJECT

        bool isVideoModeSupported_;
        OGResourceManager* pResourceManager_;

    protected:
        static OGGameEngine* gameEngine_;
        int width_, height_;
        int frameDelay_;
        bool fullscreen_;
        OGWindow* window_;
        OGGame* pGame_;

        bool eventFilter(QObject* obj, QEvent* event);

    public:
        OGGameEngine(OGGame* game, int width, int height, bool fullscreen = false);
        virtual ~OGGameEngine();

        static OGGameEngine* getEngine() { return gameEngine_; }
        bool initialize();

        OGWindow* getWindow() const { return window_; }

        int getWidth() const { return width_; }
        int getHeight() const { return height_; }
        int getFrameDelay() const { return frameDelay_; }

        OGPhysicsEngine* getPhysicsEngine();

        OGResourceManager* getResourceManager();

        void addWindow(const QString &id, OGUIWindow* wnd);
        void RemoveWindow(const QString &id);

    public slots:
        void setFrameRate(int framerate) { frameDelay_ = qRound(1000.0 / framerate); }

    private slots:
        void gameExit();
};

#endif // OG_GAMEENGINE_H
