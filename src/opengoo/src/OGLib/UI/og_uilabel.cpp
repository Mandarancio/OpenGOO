#include "og_uilabel.h"

#include <QPainter>

using namespace og::ui;

struct Label::Impl
{
    QFont font;
    QColor fontColor;
    QString text;
};

Label::Label() : _pImpl(new Impl)
{    
    setFont(QFont("Arial", 14, QFont::Bold));
    setFontColor(Qt::white);
    setBgColor(Qt::transparent);
}

// Dtor prevents error C2027
Label::~Label() {}

void Label::_Paint(QPainter* painter)
{
    Frame::_Paint(painter);

    painter->save();

    painter->setPen(_pImpl->fontColor);
    painter->setFont(_pImpl->font);
    painter->drawText(rect(), Qt::AlignLeft, _pImpl->text);

    painter->restore();
}

void Label::setFont(const QFont &font)
{
    _pImpl->font = font;
}

void Label::setFontColor(const QColor &color)
{
    _pImpl->fontColor = color;
}

void Label::setText(const QString &text)
{
    _pImpl->text = text;
}
