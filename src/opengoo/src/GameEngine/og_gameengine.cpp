#include "og_gameengine.h"

#include <QGuiApplication>
#include <QScreen>
#include <QDebug>

#include "og_videomode.h"
#include "logger.h"

OGGameEngine* OGGameEngine::gameEngine_ = 0;

int main(int argc, char **argv)
{
    if (GameInitialize(argc, argv))
    {
        QGuiApplication app(argc, argv);

        app.installEventFilter(OGGameEngine::getEngine());

        if (!OGGameEngine::getEngine()->initialize())
        {
            return false;
        }

        QObject::connect(&app, SIGNAL(aboutToQuit()), OGGameEngine::getEngine(), SLOT(gameExit()));

        return app.exec();
    }

    GameEnd();

    return true;
}

OGGameEngine::OGGameEngine(int width, int height, bool fullscreen)
{
    gameEngine_ = this;
    width_ = width;
    height_ = height;
    frameDelay_ = 50;   // 20 FPS default
    fullscreen_ = fullscreen;
    isVideoModeSupported_ = false;
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
    window_ = new OGWindow;

    // Set fixed size of window
    window_->setMaximumSize(QSize(getWidth(), getHeight()));
    window_->setMinimumSize(QSize(getWidth(), getHeight()));

    // Center window on screen
    int x = qRound((screen->geometry().width() - getWidth()) / 2.);
    int y = qRound((screen->geometry().height() - getHeight()) /2.);
    window_->setGeometry(x, y, getWidth(), getHeight());

    window_->setKeyboardGrabEnabled(true);
    connect(&gameCycle_, SIGNAL(timeout()), this, SLOT(gameLoop()));

    if (fullscreen_)
    {
    //initialize video mode
#ifdef Q_OS_WIN32
        isVideoModeSupported_ =
                OGVideoMode::testVideoMode(
                    getWidth(),
                    getHeight()
                    );

        if (isVideoModeSupported_)
        {
            OGVideoMode::setVideoMode(
                        getWidth(),
                        getHeight()
                        );

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

bool OGGameEngine::eventFilter(QObject *obj, QEvent *event)
{
    switch (event->type())
    {
    case QEvent::ApplicationActivate:
        GameActivate();
        gameCycle_.start(frameDelay_);

        return true;

    case QEvent::ApplicationDeactivate:
        gameCycle_.stop();
        GameDeactivate();

        return true;
    }

    return QObject::eventFilter(obj, event);
}

void OGGameEngine::changeFrameRate()
{
    gameCycle_.stop();
    gameCycle_.start(frameDelay_);
}

void OGGameEngine::gameExit()
{
    GameEnd();
    QGuiApplication::quit();
}

void OGGameEngine::gameLoop()
{
    GameCycle();
}
