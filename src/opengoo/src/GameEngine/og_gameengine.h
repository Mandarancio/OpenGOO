#pragma once

#include <QObject>

#include "og_widget.h"
#include "PhysicsEngine/og_physicsengine.h"
#include "iresourcemanager.h"
#include "iresourcemanagerfactory.h"
#include "og_game.h"

typedef og::OGWidget OGWindow;

class QScreen;
struct OGConfig;

namespace og
{
    class OGPhysicsEngine;

    class OGGameEngine : public QObject
    {
            Q_OBJECT

            bool m_isVideoModeSupported;
            std::unique_ptr<IResourceManager> m_resourceManager;
            std::unique_ptr<OGWindow> m_window;

            QScreen* getPrimaryScreen();

            Q_DISABLE_COPY(OGGameEngine)

        protected:
            static OGGameEngine* m_instance;
            int m_width, m_height;
            int m_frameRate;
            bool m_fullscreen;
            bool m_crt;
            OGGame* m_game;

            bool eventFilter(QObject* a_obj, QEvent* a_event);

        public:
            OGGameEngine(OGGame* a_game, const OGConfig& a_config, IResourceManagerFactory& aFactory);

            virtual ~OGGameEngine();

            static OGGameEngine* getInstance()
            {
                return m_instance;
            }

            bool initialize();

            int getWidth()
            {
                return m_width;
            }

            int getHeight()
            {
                return m_height;
            }

            int getFrameDelay()
            {
                return qRound(1000.0f / m_frameRate);
            }

            int getFrameRate()
            {
                return m_frameRate;
            }

            IResourceManager* getResourceManager()
            {
                return m_resourceManager.get();
            }

            void addUI(ui::IUI* a_ui);
            void removeUI(ui::IUI* a_ui);

            OGWindow* getWindow()
            {
                return m_window.get();
            }

            bool isCrt()
            {
                return m_crt;
            }

            void setFrameRate(int a_frameRate)
            {
                m_frameRate = a_frameRate;
            }

            void setBackgroundColor(const QColor& color)
            {
                m_window->setBackgroundColor(color, true);
            }

            Camera* getCamera()
            {
                return m_game->GetCamera();
            }

            QPoint windowToLogical(const QPoint& p);

        public slots:
            void quit();

        private slots:
            void gameExit();
    };
}

#define GE og::OGGameEngine::getInstance()
