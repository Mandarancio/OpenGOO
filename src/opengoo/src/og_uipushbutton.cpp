#include "og_uipushbutton.h"

#include <QPainter>

struct OGUIPushButtonImpl
{
    QString text;
    QFont font;
    QColor fontColor;
};

OGUIPushButton::OGUIPushButton()
{
    pImpl_ = new OGUIPushButtonImpl;
    pImpl_->font = QFont("Times", 14, QFont::Bold);
    pImpl_->fontColor = QColor(Qt::black);
    SetBG(QColor(236, 233, 216));
}

OGUIPushButton::~OGUIPushButton()
{
    delete pImpl_;
}

void OGUIPushButton::SetFontColor(int r, int g, int b, int a)
{
    SetFontColor(QColor(r, g, b, a));
}

void OGUIPushButton::SetFontColor(const QColor & color)
{
    pImpl_->fontColor = color;
}

void OGUIPushButton::SetText(const QString &text)
{
    pImpl_->text = text;
}

void OGUIPushButton::_Paint(QPainter *painter)
{
    QRect target(GetAbsPosition(), GetSize());

    painter->save();

    painter->setPen(GetBG());
    painter->setBrush(GetBG());
    painter->drawRect(target);

    painter->setPen(pImpl_->fontColor);
    painter->setFont(pImpl_->font);
    painter->drawText(target, Qt::AlignCenter, pImpl_->text);

    painter->restore();
}
