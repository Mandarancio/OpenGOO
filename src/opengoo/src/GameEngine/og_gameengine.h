#ifndef OG_GAMEENGINE_H
#define OG_GAMEENGINE_H

#include <QObject>
#include <QTimer>

#include "og_window.h"

int main(int argc, char **argv);
bool GameInitialize(int argc, char **argv);
void GameStart();
void GameEnd();
void GameActivate();
void GameDeactivate();
void GamePaint(QPainter* painter);
void GameCycle();
void HandleKeys(QKeyEvent *event);
void MouseButtonDown(QMouseEvent *event);
void MouseButtonUp(QMouseEvent *event);
void MouseMove(QMouseEvent* event);
void MouseWheel(QWheelEvent* event);

class OGGameEngine : public QObject
{
    Q_OBJECT

    bool isVideoModeSupported_;

protected:
    static OGGameEngine* gameEngine_;
    int width_, height_;
    int frameDelay_;
    bool fullscreen_;
    OGWindow* window_;
    QTimer gameCycle_;

    bool eventFilter(QObject *obj, QEvent *event);

public:
    OGGameEngine(int width = 640, int height = 480, bool fullscreen=false);
    virtual ~OGGameEngine();

    static OGGameEngine* getEngine() { return gameEngine_; }
    bool initialize();

    OGWindow* getWindow() const { return window_; }

    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    int getFrameDelay() const { return frameDelay_; }

public slots:
    void setFrameRate(int framerate) { frameDelay_ = qRound(1000.0 /framerate); }
    void changeFrameRate();
    void gameLoop();
    void gameExit();
};

#endif // OG_GAMEENGINE_H
