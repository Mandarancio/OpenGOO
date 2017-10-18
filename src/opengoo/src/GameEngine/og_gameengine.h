#pragma once

#include <QObject>
#include <memory>

#include "og_widget.h"
#include "PhysicsEngine/og_physicsengine.h"
#include "og_resourcemanager.h"


typedef og::OGWidget OGWindow;

class QScreen;
struct OGConfig;

namespace og
{
    class OGGame;
    class OGPhysicsEngine;

    class OGGameEngine : public QObject
    {
            Q_OBJECT

            bool m_isVideoModeSupported;
            std::unique_ptr<OGResourceManager> m_resourceManager;
            std::unique_ptr<OGWindow> m_window;

            QScreen* getPrimaryScreen();

            Q_DISABLE_COPY(OGGameEngine)

        protected:
            static OGGameEngine* m_instance;
            int m_width, m_height;
            int m_frameDelay;
            bool m_fullscreen;
            bool m_crt;
            OGGame* m_game;

            bool eventFilter(QObject* a_obj, QEvent* a_event);

        public:
            OGGameEngine(OGGame* a_game, const OGConfig& a_config);

            virtual ~OGGameEngine();

            static OGGameEngine* getInstance() { return m_instance; }
            bool initialize();

            int getWidth() const { return m_width; }
            int getHeight() const { return m_height; }
            int getFrameDelay() const { return m_frameDelay; }

            OGResourceManager* getResourceManager();

            void addUI(ui::IUI* a_ui);
            void removeUI(ui::IUI* a_ui);

            OGWindow* getWindow() const { return m_window.get(); }

            bool isCrt() const
            {
                return m_crt;
            }

        public slots:
            void setFrameRate(int a_framerate) { m_frameDelay = qRound(1000.0f / a_framerate); }
            void quit();

        private slots:
            void gameExit();
    };
}

#define GE og::OGGameEngine::getInstance()
