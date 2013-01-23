#include "og_gameengine.h"

#include <cstdio>

#include <QSurfaceFormat>
#include <QDebug>

OGWindow::OGWindow(QWindow* parent)
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

    if (!isExposed()) { return; }

    render();
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

    render();
}

bool OGWindow::initOpenGL_()
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

void OGWindow::render()
{
    if (!isExposed()) { return; }

    if (!initOpenGL_())
    {
        delete paintDevice_;
        paintDevice_ = 0;

        return;
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
