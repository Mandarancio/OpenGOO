#include "og_gameengine.h"
#include "og_resourcemanager.h"
#include "PhysicsEngine/og_physicsengine.h"
#include "og_game.h"

#include <QApplication>
#include <QScreen>

#include "og_videomode.h"
#include "logger.h"

using namespace og;

OGGameEngine* OGGameEngine::m_instance = 0;

OGGameEngine::OGGameEngine(OGGame* a_game, int a_width, int a_height
                           , bool a_fullscreen)
{
    m_instance = this;
    m_width = a_width;
    m_height = a_height;
    m_frameDelay = 50;   // 20 FPS default
    m_fullscreen = a_fullscreen;
    m_isVideoModeSupported = false;
    m_game = a_game;
    m_resourceManager = 0;
}

OGGameEngine::~OGGameEngine()
{
#ifdef Q_OS_WIN32
    if (m_isVideoModeSupported)
    {
        OGVideoMode::returnDefaultMode();
    }
#endif // Q_OS_WIN32
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
        //initialize video mode
#ifdef Q_OS_WIN32
        m_isVideoModeSupported = OGVideoMode::testVideoMode(getWidth()
                                , getHeight());

        if (m_isVideoModeSupported)
        {
            OGVideoMode::setVideoMode(getWidth(), getHeight());

            m_window->showFullScreen();
        }
        else { logWarn("Video mode not supported"); }
#else
        m_window->show();
#endif // Q_OS_WIN32
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

OGPhysicsEngine* OGGameEngine::getPhysicsEngine()
{
    return OGPhysicsEngine::GetInstance();
}

OGResourceManager* OGGameEngine::getResourceManager()
{
    if (!m_resourceManager) m_resourceManager = new OGResourceManager;

    return m_resourceManager;
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
