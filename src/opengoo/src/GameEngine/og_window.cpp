#include "og_gameengine.h"

#include <cstdio>

#include <QSurfaceFormat>
#include <QDebug>

OGWindow::OGWindow(QWindow* parent)
    : QWindow(parent)
{
    isGameStarted_ = false;
    isVideoInit_ = false;    
    setSurfaceType(QWindow::OpenGLSurface);

    Qt::WindowFlags flags = Qt::Window;
    flags |= Qt::WindowTitleHint;
    flags |= Qt::WindowSystemMenuHint;
    flags |= Qt::WindowMinimizeButtonHint;
    flags |= Qt::WindowCloseButtonHint;

    setFlags(flags);
}

OGWindow::~OGWindow()
{
    delete context_;
    delete gFunctions_;
    delete paintDevice_;
}

void OGWindow::keyPressEvent(QKeyEvent* event)
{
    HandleKeys(event);
}

void OGWindow::keyReleaseEvent(QKeyEvent *event)
{
    HandleKeys(event);
}

void OGWindow::exposeEvent(QExposeEvent* event)
{
    Q_UNUSED(event);

    if (!isExposed()) { return; }

    render();
}

void OGWindow::showEvent(QShowEvent* event)
{
    Q_UNUSED(event);

    if (!isGameStarted_)
    {
        GameStart();
        isGameStarted_ = true;
    }
}

void OGWindow::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event);

    if (!isExposed()) { return; }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    render();
}

QPainter* OGWindow::getPainter()
{
    if (isVideoInit_)
    {
        context_->makeCurrent(this);
        paintDevice_->setSize(QSize(width(), height()));
        QPainter* painter = new QPainter(paintDevice_);

        return painter;
    }
    else { return 0; }
}

void OGWindow::swapBuffers()
{
    if (isVideoInit_)
    {
        context_->swapBuffers(this);
    }
}

bool OGWindow::initOpenGL_()
{
    context_ = new QOpenGLContext(this);
    context_->setFormat(requestedFormat());
    context_->create();
    context_->makeCurrent(this);

    gFunctions_ = new QOpenGLFunctions(context_);
    gFunctions_->initializeOpenGLFunctions();

    paintDevice_ = new QOpenGLPaintDevice;
    paintDevice_->setSize(QSize(width(), height()));

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    return true;
}

void OGWindow::render()
{
    QPainter painter;

    if (!isVideoInit_)
    {
        if (initOpenGL_())
        {
            isVideoInit_ = true;
            painter.begin(paintDevice_);
        }
        else { return; }
    }
    else
    {
        context_->makeCurrent(this);
        paintDevice_->setSize(QSize(width(), height()));
        painter.begin(paintDevice_);
    }

    GamePaint(&painter);

    context_->swapBuffers(this);

    painter.end();
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
