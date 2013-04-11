#include "og_gameengine.h"
#include "og_game.h"

#include <cstdio>

#include <QSurfaceFormat>
#include <QDebug>

OGWindow::OGWindow(OGGame* game, QWindow* parent)
    : QWindow(parent)
{
    setSurfaceType(QWindow::OpenGLSurface);

    Qt::WindowFlags flags = Qt::Window;
    flags |= Qt::WindowTitleHint;
    flags |= Qt::WindowSystemMenuHint;
    flags |= Qt::WindowMinimizeButtonHint;
    flags |= Qt::WindowCloseButtonHint;

    setFlags(flags);

    pPaintDevice_ = 0;
    pContext_ = 0;
    pTimer_ = 0;

    pGame_ = game;
}

OGWindow::~OGWindow()
{
    delete pPaintDevice_;
    delete pContext_;
    delete pTimer_;
}

void OGWindow::keyPressEvent(QKeyEvent* event)
{
    pGame_->KeyDown(event);
}

void OGWindow::keyReleaseEvent(QKeyEvent* event)
{
    pGame_->KeyUp(event);
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

    pGame_->Start();

    if (!pTimer_)
    {
        pTimer_ = new QTimer();
        QObject::connect(pTimer_, SIGNAL(timeout()), this, SLOT(processing()));
        pTimer_->start();
    }
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

    if (!pContext_)
    {
        pContext_ = new QOpenGLContext(this);
        pContext_->setFormat(requestedFormat());
        pContext_->create();

        needsInitialize = true;
    }

    if (!pContext_->makeCurrent(this))
    {
        delete pContext_;
        pContext_ = 0;

        return false;
    }

    if (needsInitialize)
    {
        glClear(GL_COLOR_BUFFER_BIT
                | GL_DEPTH_BUFFER_BIT
                | GL_STENCIL_BUFFER_BIT);
    }

    return true;
}

void OGWindow::draw()
{
    if (!isExposed()) { return; }

    if (pContext_ == 0)
    {
        if (!_initOpenGL())
        {
            delete pPaintDevice_;
            pPaintDevice_ = 0;

            return;
        }
    }

    if (!pPaintDevice_)
    {
        pPaintDevice_ = new QOpenGLPaintDevice(QSize(width(), height()));
    }

    QPainter painter(pPaintDevice_);
    pGame_->Paint(&painter);

    pContext_->swapBuffers(this);
}

void OGWindow::mousePressEvent(QMouseEvent* event)
{
    pGame_->MouseButtonDown(event);
}

void OGWindow::mouseReleaseEvent(QMouseEvent* event)
{
    pGame_->MouseButtonUp(event);
}

void OGWindow::mouseMoveEvent(QMouseEvent* event)
{
    pGame_->MouseMove(event);
}

void OGWindow::wheelEvent(QWheelEvent* event)
{
    pGame_->MouseWheel(event);
}

void OGWindow::processing()
{
    pGame_->Cycle();
    draw();
}

void OGWindow::setActive(bool active)
{
    if (!pTimer_) return;

    if (active) pTimer_->start();
    else pTimer_->stop();
}
