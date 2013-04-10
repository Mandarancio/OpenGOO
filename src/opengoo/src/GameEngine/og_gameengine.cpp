#include "og_gameengine.h"

#include <QGuiApplication>
#include <QScreen>

#include "og_videomode.h"
#include "logger.h"

OGGameEngine* OGGameEngine::gameEngine_ = 0;

OGGameEngine::OGGameEngine(OGGame* game, int width, int height, bool fullscreen)
{
    gameEngine_ = this;
    width_ = width;
    height_ = height;
    frameDelay_ = 50;   // 20 FPS default
    fullscreen_ = fullscreen;
    isVideoModeSupported_ = false;
    pGame_ = game;
}

OGGameEngine::~OGGameEngine()
{
#ifdef Q_OS_WIN32
    if (isVideoModeSupported_)
    {
        OGVideoMode::returnDefaultMode();
    }
#endif // Q_OS_WIN32
    delete window_;
}

bool OGGameEngine::initialize()
{
    QScreen* screen = QGuiApplication::primaryScreen();
    window_ = new OGWindow(0);

    // Set fixed size of window
    window_->setMaximumSize(QSize(getWidth(), getHeight()));
    window_->setMinimumSize(QSize(getWidth(), getHeight()));

    // Center window on screen
    int x = qRound((screen->geometry().width() - getWidth()) / 2.0f);
    int y = qRound((screen->geometry().height() - getHeight()) / 2.0f);
    window_->setGeometry(x, y, getWidth(), getHeight());

    if (fullscreen_)
    {
        //initialize video mode
#ifdef Q_OS_WIN32
        isVideoModeSupported_ = OGVideoMode::testVideoMode(getWidth()
                                , getHeight());

        if (isVideoModeSupported_)
        {
            OGVideoMode::setVideoMode(getWidth(), getHeight());

            window_->showFullScreen();
        }
        else { logWarn("Video mode not supported"); }
#else
        window_->show();
#endif // Q_OS_WIN32
    }
    else { window_->show(); }

    return true;
}

bool OGGameEngine::eventFilter(QObject* obj, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::ApplicationActivate:
            GameActivate();            
            getWindow()->setActive(true);
            getWindow()->setKeyboardGrabEnabled(true);

            return true;

        case QEvent::ApplicationDeactivate:
            getWindow()->setActive(false);
            GameDeactivate();
            getWindow()->setKeyboardGrabEnabled(false);

            return true;
        default:

            break;
    }

    return QObject::eventFilter(obj, event);
}

void OGGameEngine::gameExit()
{
    getWindow()->setActive(false);
    GameEnd();
    QGuiApplication::quit();
}
