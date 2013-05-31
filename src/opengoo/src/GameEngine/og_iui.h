#ifndef OG_IUI_H
#define OG_IUI_H

#include <QList>
#include <QRect>

class QPainter;
class QMouseEvent;

namespace og {
namespace ui {
class IUI
{
public:
    virtual ~IUI() {}

    int x() const { return _x(); }
    int y() const { return _y(); }

    int height() const { return _height(); }
    int width() const { return _width(); }

    QRect rect() const { return _rect(); }

    void setPosition(int xpos, int ypos) { _setPosition(xpos, ypos); }
    void setSize(int width, int height) { _setSize(width, height); }

    bool isVisible() const { return _isVisible(); }
    void setVisible(bool visible) { _setVisible(visible); }

    void Paint(QPainter* painter) { _Paint(painter); }

    void onMouseDown(QMouseEvent* ev) { _onMouseDown(ev); }
    void onMouseUp(QMouseEvent* ev) { _onMouseUp(ev); }
    void mouseMove(QMouseEvent* ev) { _mouseMove(ev); }

private:
    virtual int _x() const = 0;
    virtual int _y() const = 0;
    virtual int _height() const = 0;
    virtual int _width() const = 0;

    virtual QRect _rect() const = 0;

    virtual void _setPosition(int xpos, int ypos) = 0;
    virtual void _setSize(int width, int height) = 0;

    virtual bool _isVisible() const = 0;
    virtual void _setVisible(bool visible) = 0;

    virtual void _Paint(QPainter *painter) = 0 ;

    virtual void _onMouseDown(QMouseEvent* ev) = 0;
    virtual void _onMouseUp(QMouseEvent* ev) = 0;
    virtual void _mouseMove(QMouseEvent* ev) = 0;
};

    typedef QList<IUI*> UIList;

} // namespace ui

} // namespace og

#endif // OG_IUI_H
