#ifndef OG_BUTTON_H
#define OG_BUTTON_H

#include "og_sprite.h"

#include <QPainter>

class OGButton
{
    OGSprite* up_;
    OGSprite* over_;
    OGSprite* disabled_;
    QString onclick_;
    QString onmouseenter_;
    QString onmouseexit_;
    QPointF position_;
    QSize size_;

public:
    OGButton() : up_(0), over_(0), disabled_(0) {}
    virtual ~OGButton() {}

    OGSprite* up() const { return up_; }
    OGSprite* over() const { return over_; }
    OGSprite* disabled() const { return disabled_; }
    QString onclick() const { return onclick_; }
    QString onmouseEnter() const { return onmouseenter_; }
    QString onmouseExit() const { return onmouseexit_; }
    QPointF position() const { return position_; }
    QSize size() const { return size_; }

    void up(OGSprite* up) { up_ = up; }
    void over(OGSprite* over) { over_ = over; }
    void disabled(OGSprite* disabled) { disabled_ = disabled; }
    void onclick(const QString & onclick) { onclick_ = onclick; }
    void onmouseEnter(const QString & onmouseenter) { onmouseenter_ = onmouseenter ;}
    void onmouseExit(const QString & onmouseexit) { onmouseexit_ = onmouseexit; }
    void position(const QPointF position) { position_ = position; }
    void size(const QSize & size) { size_ = size; }    

    bool TestPoint(const QPoint& pos)
    {
        QRectF rect(QPointF(position().x(), position().y()), size());
        rect.moveCenter(position());
        return rect.contains(pos);
    }

    QString getLevelName()
    {
        QString name;

        if (onclick_.startsWith("pl_"))
        {
            name = onclick_.right(onclick_.size() - 3);
        }

        return name;
    }
};

#endif // OG_BUTTON_H
