#include "og_widget.h"
#include <QMouseEvent>
#include "og_game.h"
#include "og_gameengine.h"

using namespace og;

OGWidget::OGWidget(OGGame* game)
{
    _pGame = game;

    setWindowFlags(Qt::WindowCloseButtonHint);

    setMouseTracking(true);
    setAutoFillBackground(false);

    connect(&_timer, SIGNAL(timeout()), this, SLOT(Update()));
}

void OGWidget::setActive(bool active)
{
    if (active)
        _timer.start(GE->getFrameDelay());
    else
        _timer.stop();
}

ui::UIList& OGWidget::uiList() { return _uiList; }

const ui::UIList& OGWidget::uiList() const { return _uiList; }

int OGWidget::findUI(ui::IUI* ui)
{
    if (uiList().isEmpty()) return -1;

    for (int i = 0; i < uiList().size(); i++)
    {
        if (ui == uiList().at(i)) return i;
    }

    return -1;
}

void OGWidget::addUI(ui::IUI* ui)
{
    if (findUI(ui) == -1) uiList().append(ui);
}

void OGWidget::removeUI(ui::IUI* ui)
{
    int i = findUI(ui);

    if (i != -1) uiList().removeAt(i);
}

void OGWidget::Update()
{
    getGame()->Cycle();

    repaint();
}

void OGWidget::keyReleaseEvent(QKeyEvent* ev)
{
    getGame()->KeyUp(ev);
}

void OGWidget::keyPressEvent(QKeyEvent* ev)
{
    getGame()->KeyDown(ev);
}

void OGWidget::showEvent(QShowEvent* ev)
{
    Q_UNUSED(ev)

    getGame()->Start();
    _timer.start(GE->getFrameDelay());
}

void OGWidget::resizeEvent(QResizeEvent* ev)
{
    Q_UNUSED(ev)
}

void OGWidget::mousePressEvent(QMouseEvent* ev)
{
    if (!uiList().isEmpty())
    {
        for (int i = uiList().size() - 1; i >= 0; i--)
        {
            if (uiList().at(i)->isVisible())
            {
                uiList().at(i)->onMouseDown(ev);

                if (ev->isAccepted()) return;
            }
        }
    }

    getGame()->MouseButtonDown(ev);

    ev->accept();
}

void OGWidget::mouseReleaseEvent(QMouseEvent* ev)
{
    if (!uiList().isEmpty())
    {
        for (int i = uiList().size() - 1; i >= 0; i--)
        {
            if (uiList().at(i)->isVisible())
            {
                uiList().at(i)->onMouseUp(ev);

                if (ev->isAccepted()) return;
            }
        }
    }

    getGame()->MouseButtonUp(ev);

    ev->accept();
}

void OGWidget::mouseMoveEvent(QMouseEvent* ev)
{
    if (!uiList().isEmpty())
    {
        for (int i = uiList().size() - 1; i >= 0; i--)
        {
            if (uiList().at(i)->isVisible())
            {
                uiList().at(i)->mouseMove(ev);

                if (ev->isAccepted()) return;
            }
        }
    }

    getGame()->MouseMove(ev);

    ev->accept();
}

void OGWidget::wheelEvent(QWheelEvent* ev)
{
    getGame()->MouseWheel(ev);
}

void OGWidget::paintEvent(QPaintEvent *ev)
{
    Q_UNUSED(ev)

    QPainter painter;

    painter.begin(this);

    painter.setRenderHint(QPainter::SmoothPixmapTransform);

    getGame()->Paint(&painter);

    painter.setWindow(0, 0, width(), height());

    if (!uiList().isEmpty())
    {
        for (int i = 0; i < uiList().size(); i++)
        {
            if (uiList().at(i)->isVisible()) uiList().at(i)->Paint(&painter);
        }
    }

    painter.end();
}

void OGWidget::setBackgroundColor(const QColor& col, bool show)
{
    auto p = palette();
    p.setBrush(QPalette::Window, col);
    setPalette(p);
    setAutoFillBackground(show);
}
