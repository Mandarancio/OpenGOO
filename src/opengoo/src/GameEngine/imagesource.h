#pragma once

#include <memory>

#include <QPixmap>

class QRectF;
class QPointF;
class QImage;

namespace og
{
class ImageSource
{
    QPixmap m_image;

public:
    ImageSource()
    {
    }

    ImageSource(const QString& a_filename);

    ImageSource(const QImage& aImage);

    void Render(QPainter& a_painter, const QRectF& a_target, const QRectF& a_source);

    void Render(QPainter& a_painter, const QPointF& a_pos, const QRectF& a_source);

    int GetWidth() const
    {
        return m_image.width();
    }

    int GetHeight() const
    {
        return m_image.height();
    }
};

typedef std::shared_ptr<ImageSource> ImageSourceSPtr;
}
