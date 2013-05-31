#include "og_uiframe.h"

#include <QPainter>

using namespace og::ui;

struct Frame::Impl
{
    QColor bgColor;
};

Frame::Frame() : _pImpl(new Impl) {}

// Dtor prevents error C2027
Frame::~Frame() {}

void Frame::setBgColor(const QColor &color)
{
    _pImpl->bgColor = color;
}

void Frame::_Paint(QPainter* painter)
{
    painter->save();

    painter->fillRect(rect(), _pImpl->bgColor);

    painter->restore();
}
