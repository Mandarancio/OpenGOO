#include <QMouseEvent>

#include "og_widget.h"
#include "og_game.h"
#include "og_gameengine.h"
#include "mouselistener.h"

namespace og
{
class OGWidget::Input : public MouseListener
{
public:
    Input()
    {
        mMouseButton.fill(false);
    }

    bool IsButtonPressed(MouseInput::MouseButton aButton)
    {
        switch (aButton)
        {
        case MouseInput::e_left:
        case MouseInput::e_middle:
        case MouseInput::e_right:
            return mMouseButton[aButton];
        case MouseInput::e_none:
            return std::none_of(mMouseButton.begin(), mMouseButton.end(), [](bool btn) { return btn; });
        case MouseInput::e_any:
            return std::any_of(mMouseButton.begin(), mMouseButton.end(), [](bool btn) { return btn; });
        }

        return false;
    }

    void OnMouseDown(QMouseEvent* aEv)
    {
        switch (aEv->button())
        {
        case Qt::LeftButton:
            mMouseButton[MouseInput::e_left] = true;
            break;
        case Qt::MiddleButton:
            mMouseButton[MouseInput::e_middle] = true;
            break;
        case Qt::RightButton:
            mMouseButton[MouseInput::e_right] = true;
            break;
        default:
            break;
        }
    }

    void OnMouseUp(QMouseEvent* aEv)
    {
        switch (aEv->button())
        {
        case Qt::LeftButton:
            mMouseButton[MouseInput::e_left] = false;
            break;
        case Qt::MiddleButton:
            mMouseButton[MouseInput::e_middle] = false;
            break;
        case Qt::RightButton:
            mMouseButton[MouseInput::e_right] = false;
            break;
        default:
            break;
        }
    }

    void OnMouseMove(QMouseEvent* /*aEv*/)
    {
    }

private:
     std::array<bool, 3> mMouseButton;
};

void OGWidget::InputDeleter::operator()(Input* ptr) const
{
    delete ptr;
}

OGWidget::OGWidget(OGGame* game)
    : mInput(new Input)
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

ui::UIList& OGWidget::uiList()
{
    return _uiList;
}

const ui::UIList& OGWidget::uiList() const
{
    return _uiList;
}

int OGWidget::findUI(ui::IUI* ui)
{
    if (uiList().isEmpty())
        return -1;

    for (int i = 0; i < uiList().size(); i++)
    {
        if (ui == uiList().at(i))
            return i;
    }

    return -1;
}

void OGWidget::addUI(ui::IUI* ui)
{
    if (findUI(ui) == -1)
        uiList().append(ui);
}

void OGWidget::removeUI(ui::IUI* ui)
{
    int i = findUI(ui);

    if (i != -1)
        uiList().removeAt(i);
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

void OGWidget::showEvent(QShowEvent*)
{
    getGame()->Start();
    _timer.start(GE->getFrameDelay());
}

void OGWidget::resizeEvent(QResizeEvent*)
{
}

void OGWidget::mousePressEvent(QMouseEvent* aEv)
{
    mInput->OnMouseDown(aEv);

    getGame()->MouseButtonDown(GE->windowToLogical(aEv->pos()));

    aEv->accept();
}

void OGWidget::mouseReleaseEvent(QMouseEvent* aEv)
{
    mInput->OnMouseUp(aEv);

    getGame()->MouseButtonUp(GE->windowToLogical(aEv->pos()));

    aEv->accept();
}

void OGWidget::mouseMoveEvent(QMouseEvent* aEv)
{
    getGame()->MouseMove(GE->windowToLogical(aEv->pos()));

    aEv->accept();
}

void OGWidget::wheelEvent(QWheelEvent* ev)
{
    getGame()->MouseWheel(ev);
}

void OGWidget::paintEvent(QPaintEvent*)
{
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    if (GE->isCrt())
        painter.setViewport(0, 0, GE->getWidth(), GE->getHeight());
    else
        painter.setViewport(0, 0, width(), height());

    getGame()->Paint(&painter);

    if (!uiList().isEmpty())
    {
        for (int i = 0; i < uiList().size(); i++)
        {
            if (uiList().at(i)->isVisible())
                uiList().at(i)->Paint(&painter);
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

bool OGWidget::IsButtonPressed(MouseInput::MouseButton aButton) const
{
    return mInput->IsButtonPressed(aButton);
}

QPoint OGWidget::GetMousePosition() const
{
    return mapFromGlobal(cursor().pos());
}
}
