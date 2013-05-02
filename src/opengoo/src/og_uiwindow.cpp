#include "og_uiwindow.h"
#include "og_ui.h"

#include <QMouseEvent>
#include <QPainter>
#include <QRect>

#include <QDebug>

struct OGUIWindowImpl
{
    OGUIWindow* pParent;
    QPoint pos;
    QSize size;
    QColor bg;
    OGUI* pUI;
    bool isShow;
};

OGUIWindow::OGUIWindow(OGUIWindow* parent)
{
    pImpl_ = new OGUIWindowImpl;
    pImpl_->pParent = parent;
    pImpl_->pos = QPoint(0, 0);
    pImpl_->size = QSize(100, 40);
    pImpl_->bg = QColor(157, 172, 189);
    pImpl_->pUI = 0;
    pImpl_->isShow = false;
}

OGUIWindow::~OGUIWindow()
{
    delete pImpl_;
}

void OGUIWindow::AddUI(OGUI *ui)
{
    pImpl_->pUI = ui;
}

inline QPoint OGUIWindow::GetAbsPosition()
{
    QPoint pos = pImpl_->pos;
    OGUIWindow* p = pImpl_->pParent;

    while(p)
    {
        pos += p->pImpl_->pos;
        p = p->pImpl_->pParent;
    }

    return pos;
}

const QColor &OGUIWindow::GetBG() const
{
    return pImpl_->bg;
}

QRect OGUIWindow::GetGeometry() const
{
    return QRect(pImpl_->pos, pImpl_->size);
}

const OGUIWindow* OGUIWindow::GetParent() const
{
    return pImpl_->pParent;
}

const QPoint &OGUIWindow::GetPosition() const
{
    return pImpl_->pos;
}

const QSize &OGUIWindow::GetSize() const
{
    return pImpl_->size;
}

void OGUIWindow::MoveCenter(const QPoint &pos)
{
    int x = pos.x() - pImpl_->size.width() / 2;
    int y = pos.y() - pImpl_->size.height() / 2;
    pImpl_->pos = QPoint(x, y);
}

void OGUIWindow::MoveCenter(int posx, int posy)
{
    QRect rect(pImpl_->pos, pImpl_->size);
    rect.moveCenter(QPoint(posx, posy));
    pImpl_->pos = rect.topLeft();
}

void OGUIWindow::Hide()
{
    pImpl_->isShow = false;
}

void OGUIWindow::Show()
{
    pImpl_->isShow = true;
}

bool OGUIWindow::isShow()
{
    return pImpl_->isShow;
}

void OGUIWindow::SetBG(const QColor &color)
{
    pImpl_->bg = color;
}

void OGUIWindow::SetParent(OGUIWindow *parent)
{
    pImpl_->pParent = parent;
}

void OGUIWindow::SetPosition(const QPoint &pos)
{
    pImpl_->pos = pos;
}

void OGUIWindow::SetPosition(int posx, int posy)
{
    pImpl_->pos = QPoint(posx, posy);
}

void OGUIWindow::SetSize(const QSize &size)
{
    pImpl_->size = size;
}

void OGUIWindow::SetSize(int width, int height)
{
    pImpl_->size = QSize(width, height);
}

void OGUIWindow::Paint(QPainter* painter)
{
    if (pImpl_->isShow) _Paint(painter);
}

bool OGUIWindow::MouseDown(QMouseEvent *ev)
{
    if (!TestPoint(ev->pos())) return false;

    bool status;

    if (pImpl_->pUI)
    {
        status = pImpl_->pUI->_MouseDown(ev);
    }
    else
    {
        status =  _MouseButtonDown(ev);
    }

    return status;
}

bool OGUIWindow::MouseButtonUp(QMouseEvent* ev)
{
    if (TestPoint(ev->pos())) return _MouseButtonUp(ev);
    else return false;
}

bool OGUIWindow::MouseMove(QMouseEvent *ev)
{
    if (TestPoint(ev->pos())) return _MouseMove(ev);
    else return false;
}

inline bool OGUIWindow::TestPoint(const QPoint &pos)
{
    QPoint position = GetAbsPosition();

    QRect rect(position, GetSize());

    if (rect.contains(pos)) return true;
    else return false;
}

inline void OGUIWindow::_Paint(QPainter* painter)
{  
    painter->save();

    painter->setPen(pImpl_->bg);
    painter->setBrush(pImpl_->bg);

    QPoint pos = GetAbsPosition();

    painter->drawRect(QRect(pos, pImpl_->size));

    painter->restore();

    if (pImpl_->pUI) pImpl_->pUI->Paint(painter);
}

inline bool OGUIWindow::_MouseButtonDown(QMouseEvent* ev)
{
    Q_UNUSED(ev)

    return true;
}

inline bool OGUIWindow::_MouseButtonUp(QMouseEvent* ev)
{
    Q_UNUSED(ev)

    return true;
}

inline bool OGUIWindow::_MouseMove(QMouseEvent* ev)
{
    Q_UNUSED(ev)

    return true;
}

