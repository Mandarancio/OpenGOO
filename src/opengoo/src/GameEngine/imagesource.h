#pragma once

#include <QPixmap>

class QRectF;
class QPointF;

namespace og
{
class ImageSource
{
    QPixmap m_image;

public:
    ImageSource()
    {
    }

    ImageSource(const QString& a_filename)
        : m_image(a_filename)
    {
    }

    void Render(QPainter& a_painter,
                const QRectF& a_target,
                const QRectF& a_source);

    void Render(QPainter& a_painter,
                const QPointF& a_pos,
                const QRectF& a_source);

    int GetWidth() const
    {
        return m_image.width();
    }

    int GetHeight() const
    {
        return m_image.height();
    }
};
}
