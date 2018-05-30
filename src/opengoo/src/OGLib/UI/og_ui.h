#ifndef OG_UI_H
#define OG_UI_H

#include <memory>
#include "og_iui.h"

namespace og {
namespace ui {
class UI : public IUI
{
public:
    UI();
    ~UI();

private:
    UI(const UI&);
    UI& operator=(const UI&);

    struct Impl;
    std::unique_ptr<Impl> _pImpl;

    int _x() const;
    int _y() const;

    int _height() const;
    int _width() const;

    QRect _rect() const;

    void _setPosition(int xpos, int ypos);
    void _setSize(int width, int height);

    bool _isVisible() const;
    void _setVisible(bool visible);

    void OnMouseDown(QMouseEvent* ev);
    void OnMouseUp(QMouseEvent* ev);
    void OnMouseMove(QMouseEvent *ev);
};

} // namespace ui

} // namespace og


#endif // OG_UI_H
