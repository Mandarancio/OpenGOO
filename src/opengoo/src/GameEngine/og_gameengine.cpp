#include "og_gameengine.h"
#include "og_resourcemanager.h"
#include "PhysicsEngine/og_physicsengine.h"
#include "og_game.h"

#include <QApplication>
#include <QScreen>

#include "og_videomode.h"
#include "logger.h"

using namespace og;

OGGameEngine* OGGameEngine::gameEngine = 0;

OGGameEngine::OGGameEngine(OGGame* game, int width, int height
                           , bool fullscreen)
{
    gameEngine = this;
    width_ = width;
    height_ = height;
    frameDelay_ = 50;   // 20 FPS default
    fullscreen_ = fullscreen;
    _isVideoModeSupported = false;
    pGame = game;
    _pResourceManager = 0;
}

OGGameEngine::~OGGameEngine()
{
#ifdef Q_OS_WIN32
    if (_isVideoModeSupported)
    {
        OGVideoMode::returnDefaultMode();
    }
#endif // Q_OS_WIN32
}

bool OGGameEngine::initialize()
{
    qApp->installEventFilter(this);

    _pWindow.reset(new OGWindow(pGame));

    QScreen* screen = _getPrimaryScreen();
    int x = (screen->geometry().width() - getWidth()) / 2.0f;
    int y = (screen->geometry().height() - getHeight()) / 2.0f;
    _pWindow->setGeometry(x, y, getWidth(), getHeight());
    _pWindow->setFixedSize(getWidth(), getHeight());

    if (fullscreen_)
    {
        //initialize video mode
#ifdef Q_OS_WIN32
        _isVideoModeSupported = OGVideoMode::testVideoMode(getWidth()
                                , getHeight());

        if (_isVideoModeSupported)
        {
            OGVideoMode::setVideoMode(getWidth(), getHeight());

            _pWindow->showFullScreen();
        }
        else { logWarn("Video mode not supported"); }
#else
        _pWindow->show();
#endif // Q_OS_WIN32
    }
    else
    {
        _pWindow->show();
    }

    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(gameExit()));

    return true;
}

bool OGGameEngine::eventFilter(QObject* obj, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::ApplicationActivate:
            pGame->Activate();
            _pWindow->setActive(true);
            _pWindow->grabKeyboard();

            return true;

        case QEvent::ApplicationDeactivate:
            _pWindow->setActive(true);
            pGame->Deactivate();
            _pWindow->releaseKeyboard();

            return true;

        default:
            break;
    }

    return QObject::eventFilter(obj, event);
}

void OGGameEngine::gameExit()
{    
    _pWindow->setActive(false);
    pGame->End();
    _pWindow.reset();
    QApplication::quit();
}

OGPhysicsEngine* OGGameEngine::getPhysicsEngine()
{
    return OGPhysicsEngine::GetInstance();
}

OGResourceManager* OGGameEngine::getResourceManager()
{
    if (!_pResourceManager) _pResourceManager = new OGResourceManager;

    return _pResourceManager;
}

void OGGameEngine::addUI(ui::IUI* ui)
{
    _pWindow->addUI(ui);
}

void OGGameEngine::removeUI(ui::IUI* ui)
{
    _pWindow->removeUI(ui);
}

QScreen* OGGameEngine::_getPrimaryScreen()
{
    return QApplication::primaryScreen();
}

void OGGameEngine::quit()
{
    _pWindow->close();
}
