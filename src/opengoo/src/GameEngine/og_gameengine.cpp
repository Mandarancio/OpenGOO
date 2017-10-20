#include "og_gameengine.h"
#include "og_game.h"
#include "og_gameconfig.h"

#include <QApplication>
#include <QScreen>

#include "og_videomode.h"
#include "logger.h"

namespace og
{
OGGameEngine* OGGameEngine::m_instance = nullptr;

OGGameEngine::OGGameEngine(OGGame* a_game, const OGConfig& a_config)
{
    m_instance = this;
    m_game = a_game;
    m_width = a_config.screen_width;
    m_height = a_config.screen_height;
    m_fullscreen = a_config.fullscreen;
    m_frameRate = a_config.refreshrate;
    m_crt = a_config.isCrt;
    m_isVideoModeSupported = false;
    m_resourceManager = nullptr;
}

OGGameEngine::~OGGameEngine()
{
    if (m_isVideoModeSupported)
    {
        OGVideoMode::returnDefaultMode();
    }
}

bool OGGameEngine::initialize()
{
    qApp->installEventFilter(this);

    m_window.reset(new OGWindow(m_game));

    QScreen* screen = getPrimaryScreen();
    int x = (screen->geometry().width() - getWidth()) / 2.0f;
    int y = (screen->geometry().height() - getHeight()) / 2.0f;
    m_window->setGeometry(x, y, getWidth(), getHeight());
    m_window->setFixedSize(getWidth(), getHeight());

    if (m_fullscreen)
    {
        if (m_crt)
        {
            m_isVideoModeSupported = OGVideoMode::testVideoMode(getWidth(), getHeight());

            if (m_isVideoModeSupported)
            {
                OGVideoMode::setVideoMode(getWidth(), getHeight());

                m_window->showFullScreen();
            }
            else
            {
                logWarn("Video mode not supported");
                m_window->show();
            }
        }
        else
        {
            m_window->showFullScreen();
        }
    }
    else
    {
        m_window->show();
    }

    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(gameExit()));

    return true;
}

bool OGGameEngine::eventFilter(QObject* a_obj, QEvent* a_event)
{
    switch (a_event->type())
    {
        case QEvent::ApplicationActivate:
            m_game->Activate();
            m_window->setActive(true);
            m_window->grabKeyboard();

            return true;

        case QEvent::ApplicationDeactivate:
            m_window->setActive(true);
            m_game->Deactivate();
            m_window->releaseKeyboard();

            return true;

        default:
            break;
    }

    return QObject::eventFilter(a_obj, a_event);
}

void OGGameEngine::gameExit()
{    
    m_window->setActive(false);
    m_game->End();
    m_window.reset();
    QApplication::quit();
}

OGResourceManager* OGGameEngine::getResourceManager()
{
    if (!m_resourceManager)
    {
        m_resourceManager.reset(new OGResourceManager);
    }

    return m_resourceManager.get();
}

void OGGameEngine::addUI(ui::IUI* a_ui)
{
    m_window->addUI(a_ui);
}

void OGGameEngine::removeUI(ui::IUI* a_ui)
{
    m_window->removeUI(a_ui);
}

QScreen* OGGameEngine::getPrimaryScreen()
{
    return QApplication::primaryScreen();
}

void OGGameEngine::quit()
{
    m_window->close();
}
}
