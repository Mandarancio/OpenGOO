#ifndef OG_WINDOWCAMERA_H
#define OG_WINDOWCAMERA_H

#include "og_camera.h"

class OGWindowCamera : public OGCamera
{
    QRect window_;
    QPoint _ToWindowCoordinates(const QPoint &center);

public:
    OGWindowCamera() : OGCamera() {}
    OGWindowCamera(const OGCamera & camera);
    OGWindowCamera(const QPoint & center, const QSize & size, float zoom
                   , int traveltime, int pause)
        : OGCamera(center, size, zoom, traveltime, pause) {}
    ~OGWindowCamera() {}

    OGWindowCamera& operator =(const OGCamera  & camera);

    QRect window() const { return window_; }
    int width() const { return window().width(); }
    int height() const { return window().height(); }

    void SetPosition(const QPoint & position);
    void SetPosition(int x, int y);
    void SetX(int x);
    void SetY(int y);

    void Zoom(float zoom);
};

#endif // OG_WINDOWCAMERA_H
