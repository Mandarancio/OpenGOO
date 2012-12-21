#include "og_gameengine.h"

#include <QGuiApplication>
#include <QDebug>

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
}

OGGameEngine::~OGGameEngine()
{
    delete window_;
}

bool OGGameEngine::initialize()
{
    window_ = new OGWindow;
    int x, y;
    x = 50;
    y = 50;
    window_->setGeometry(x, y, width_, height_);
    window_->setKeyboardGrabEnabled(true);
    connect(&gameCycle_, SIGNAL(timeout()), this, SLOT(gameLoop()));
    if (fullscreen_) { window_->showFullScreen(); }
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
