#pragma once

#include "og_ipushbutton.h"

class QPixmap;

namespace og
{
namespace ui
{
class PushButton : public IPushButton
{
    Q_OBJECT

    Q_DISABLE_COPY(PushButton)

public:
    PushButton();
    ~PushButton();

    void setDown(bool down);

signals:
    void pressed();    

protected:
    bool isOver() const;
    bool isDown() const;

    void setOver(bool over);

private:
    struct Impl;
    std::unique_ptr<Impl> _pImpl;

    void _setText(const QString &text);

    void _setFont(const QFont &font);
    void _setFontColor(const QColor & color);    

    void _setUpImage(QPixmap* pm);
    void _setOverImage(QPixmap* pm);

    void _Paint(QPainter *painter);

    void _onMouseDown(QMouseEvent* ev);
    void _onMouseUp(QMouseEvent* ev);
    void _mouseMove(QMouseEvent* ev);
};
} // ns:ui
} // ns:og
