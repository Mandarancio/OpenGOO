#ifndef OG_GAMEENGINE_H
#define OG_GAMEENGINE_H

#include <QObject>

#include "og_widget.h"

typedef og::OGWidget OGWindow;

class QScreen;

namespace og
{
    class OGGame;
    class OGPhysicsEngine;
    class OGResourceManager;

    class OGGameEngine : public QObject
    {
            Q_OBJECT

            bool _isVideoModeSupported;
            OGResourceManager* _pResourceManager;
            std::unique_ptr<OGWindow> _pWindow;

            QScreen* _getPrimaryScreen();            

            OGGameEngine(const OGGameEngine&);
            OGGameEngine& operator=(const OGGameEngine&);

        protected:
            static OGGameEngine* gameEngine;
            int width_, height_;
            int frameDelay_;
            bool fullscreen_;
            OGGame* pGame;

            bool eventFilter(QObject* obj, QEvent* event);

        public:
            OGGameEngine(OGGame* game, int width, int height
                         , bool fullscreen = false);
            virtual ~OGGameEngine();

            static OGGameEngine* getEngine() { return gameEngine; }
            bool initialize();

            int getWidth() const { return width_; }
            int getHeight() const { return height_; }
            int getFrameDelay() const { return frameDelay_; }

            OGPhysicsEngine* getPhysicsEngine();

            OGResourceManager* getResourceManager();

            void addUI(ui::IUI* ui);
            void removeUI(ui::IUI* ui);

        public slots:
            void setFrameRate(int framerate)
            { frameDelay_ = qRound(1000.0f / framerate); }
            void quit();

        private slots:
            void gameExit();
    };

} // namespace og

#endif // OG_GAMEENGINE_H
