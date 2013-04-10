#include "og_gameengine.h"

#include <cstdio>

#include <QSurfaceFormat>
#include <QDebug>

OGWindow::OGWindow(OGGame *game, QWindow *parent)
    : QWindow(parent)
{ 
    setSurfaceType(QWindow::OpenGLSurface);

    Qt::WindowFlags flags = Qt::Window;
    flags |= Qt::WindowTitleHint;
    flags |= Qt::WindowSystemMenuHint;
    flags |= Qt::WindowMinimizeButtonHint;
    flags |= Qt::WindowCloseButtonHint;

    setFlags(flags);

    paintDevice_ = 0;
    context_ = 0;

    timer_ = new QTimer();
    QObject::connect(timer_, SIGNAL(timeout()), this, SLOT(processing()));
    timer_->start();
    isActive_ = false;
    pGame_ = game;
}

OGWindow::~OGWindow()
{
    delete paintDevice_;
    delete context_;
}

void OGWindow::keyPressEvent(QKeyEvent* event)
{
    KeyDown(event);
}

void OGWindow::keyReleaseEvent(QKeyEvent* event)
{
    KeyUp(event);
}

void OGWindow::exposeEvent(QExposeEvent* event)
{
    Q_UNUSED(event);

    if (!isExposed()) return;

    draw();
}

void OGWindow::showEvent(QShowEvent* event)
{
    Q_UNUSED(event)

    GameStart();
}

void OGWindow::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);

    if (!isExposed()) { return; }

    draw();
}

bool OGWindow::_initOpenGL()
{
    bool needsInitialize = false;

    if (!context_)
    {
        context_ = new QOpenGLContext(this);
        context_->setFormat(requestedFormat());
        context_->create();

        needsInitialize = true;
    }

    if (!context_->makeCurrent(this))
    {        
        delete context_;        
        context_ = 0;

        return false;
    }

    if (needsInitialize)
    {
        glClear(GL_COLOR_BUFFER_BIT
                | GL_DEPTH_BUFFER_BIT
                |GL_STENCIL_BUFFER_BIT
                );
    }

    return true;
}

void OGWindow::draw()
{
    if (!isExposed()) { return; }

    if (context_ == 0)
    {
        if (!_initOpenGL())
        {
            delete paintDevice_;
            paintDevice_ = 0;

            return;
        }
    }

    if (!paintDevice_)
    {
        paintDevice_ = new QOpenGLPaintDevice(QSize(width(), height()));
    }

    QPainter painter(paintDevice_);
    GamePaint(&painter);

    context_->swapBuffers(this);
}

void OGWindow::mousePressEvent(QMouseEvent* event)
{
    MouseButtonDown(event);
}

void OGWindow::mouseReleaseEvent(QMouseEvent* event)
{
    MouseButtonUp(event);
}

void OGWindow::mouseMoveEvent(QMouseEvent* event)
{
    MouseMove(event);
}

void OGWindow::wheelEvent(QWheelEvent* event)
{
    MouseWheel(event);
}

void OGWindow::processing()
{
    if (!isActive_) return;

    GameCycle();
    draw();
}
