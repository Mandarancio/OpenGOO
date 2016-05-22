#include "og_ui.h"
#include "og_gameengine.h"

#include <QMouseEvent>

using namespace og::ui;

struct UI::Impl
{
    QPoint pos;
    QSize size;
    bool visible;
};

UI::UI() : _pImpl(new Impl)
{    
    _pImpl->visible = false;
    OGGameEngine::getInstance()->addUI(this);
}

UI::~UI()
{
    OGGameEngine::getInstance()->removeUI(this);
}

inline int UI::_x() const { return _pImpl->pos.x(); }
inline int UI::_y() const { return _pImpl->pos.y(); }

inline int UI::_height() const { return _pImpl->size.height(); }
inline int UI::_width() const { return _pImpl->size.width(); }

inline QRect UI::_rect() const { return QRect(_pImpl->pos, _pImpl->size); }

inline void UI::_setPosition(int xpos, int ypos)
{
    _pImpl->pos = QPoint(xpos, ypos);
}

inline void UI::_setSize(int width, int height)
{
    _pImpl->size = QSize(width, height);
}

inline bool UI::_isVisible() const { return _pImpl->visible; }
inline void UI::_setVisible(bool visible) { _pImpl->visible = visible; }

inline void UI::_onMouseDown(QMouseEvent* ev)
{
    ev->ignore();
}

inline void UI::_onMouseUp(QMouseEvent* ev)
{
    ev->ignore();
}

inline void UI::_mouseMove(QMouseEvent *ev)
{
    ev->ignore();
}
