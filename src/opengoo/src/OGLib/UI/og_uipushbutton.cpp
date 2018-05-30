#include "og_uipushbutton.h"

#include <QMouseEvent>
#include <QPainter>

using namespace og::ui;

struct PushButton::Impl
{
    QString text;
    QFont font;
    QColor fontColor;
    QPixmap* pUp;
    QPixmap* pOver;
    bool over;
    bool down;
};

PushButton::PushButton() : _pImpl(new Impl)
{    
    setFont(QFont("Arial", 14, QFont::Bold));
    setFontColor(Qt::white);
    setUpImage(0);
    setOverImage(0);
    setOver(false);
    setDown(false);
}

PushButton::~PushButton()
{
    delete _pImpl->pOver;
    delete _pImpl->pUp;
}

void PushButton::_setText(const QString &text)
{
    _pImpl->text = text;
}

void PushButton::_setFont(const QFont &font)
{
    _pImpl->font = font;
}

void PushButton::_setFontColor(const QColor & color)
{
    _pImpl->fontColor = color;
}

void PushButton::_setUpImage(QPixmap* pm)
{
    _pImpl->pUp = pm;
}

void PushButton::_setOverImage(QPixmap* pm)
{
    _pImpl->pOver = pm;
}

inline bool PushButton::isOver() const { return _pImpl->over; }
inline bool PushButton::isDown() const { return _pImpl->down; }

void PushButton::setDown(bool down) { _pImpl->down = down; }
inline void PushButton::setOver(bool over) { _pImpl->over = over; }

void PushButton::_Paint(QPainter *painter)
{
    painter->save();

    if (isOver() && _pImpl->pOver)
    {
        painter->drawPixmap(rect(), *_pImpl->pOver, _pImpl->pOver->rect());
    }
    else if (_pImpl->pUp)
    {
        painter->drawPixmap(rect(), *_pImpl->pUp, _pImpl->pUp->rect());
    }

    painter->setPen(_pImpl->fontColor);
    painter->setFont(_pImpl->font);

    painter->drawText(rect(), Qt::AlignCenter, _pImpl->text);

    painter->restore();
}

void PushButton::OnMouseDown(QMouseEvent* ev)
{
    if (hitButton(ev->pos()))
    {
        setDown(true);
        emit pressed();
        ev->accept();
    }
    else
    {
        ev->ignore();
    }
}

void PushButton::OnMouseUp(QMouseEvent* ev)
{
    if (!isDown())
    {
        ev->ignore();
        return;
    }

    if (hitButton(ev->pos()))
    {
        setDown(false);
        emit released();
        ev->accept();
    }
    else
    {
        setDown(false);
        ev->ignore();
    }
}

void PushButton::OnMouseMove(QMouseEvent* ev)
{
    if (hitButton(ev->pos()) != isOver())
    {
        setOver(!isOver());

        if (isOver())
        {
            emit over();
        }
        else
        {
            emit leave();
        }

        ev->accept();
    }
    else if (hitButton(ev->pos()))
    {
        ev->accept();
    }
    else
    {
        ev->ignore();
    }
}
