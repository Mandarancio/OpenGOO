#include <QPainter>
#include <QImage>

#include "imagesource.h"

namespace og
{
ImageSource::ImageSource(const QString& a_filename)
    : m_image(a_filename)
{
}

ImageSource::ImageSource(const QImage& aImage)
    : m_image(QPixmap::fromImage(aImage))
{
}

void ImageSource::Render(QPainter& a_painter,
            const QRectF& a_target,
            const QRectF& a_source)
{
    a_painter.drawPixmap(a_target, m_image, a_source);
}

void ImageSource::Render(QPainter& a_painter,
            const QPointF& a_pos,
            const QRectF& a_source)
{
    a_painter.drawPixmap(a_pos, m_image, a_source);
}
}
