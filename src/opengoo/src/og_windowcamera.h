#ifndef OG_WINDOWCAMERA_H
#define OG_WINDOWCAMERA_H

#include "og_camera.h"

class OGWindowCamera : public OGCamera
{
    QRectF window_;
    QPointF ToWindowCoordinates_(const QPointF & center);

public:
    OGWindowCamera() {}
    OGWindowCamera(const OGCamera & camera);
    OGWindowCamera(const QPointF & center, const QSizeF & size, qreal zoom
                   , qreal traveltime, qreal pause)
        : OGCamera(center, size, zoom, traveltime, pause) {}
    virtual ~OGWindowCamera() {}

    QRectF window() const { return window_; }
    qreal width() const { return window().width(); }
    qreal height() const { return window().height(); }

    void Scale(qreal zoom);
    void SetPosition(const QPointF & position);
    void SetPosition(qreal x, qreal y);
    void SetX(qreal x);
    void SetY(qreal y);

    OGWindowCamera& operator =(const OGCamera  & camera);
};

#endif // OG_WINDOWCAMERA_H
