#ifndef OG_CAMERA_H
#define OG_CAMERA_H

#include <QPoint>
#include <QRect>
#include "og_levelconfig.h"

class OGCamera
{
    QPoint center_;
    QSize size_;
    float zoom_;
    int traveltime_;
    int pause_;

public:
    OGCamera(const QPoint & center=QPoint(0, 0)
            , const QSize & size=QSize(800, 600)
            , float zoom=1.0f
            , int traveltime=0
            , int pause=0)
        : center_(center), size_(size), zoom_(zoom), traveltime_(traveltime)
        , pause_(pause) {}

    OGCamera(WOGPoi* poi, const QSize & size=QSize(800, 600))
        : center_(poi->position.toPoint()), size_(size), zoom_(poi->zoom)
    {
        traveltime_ = qRound(poi->traveltime * 1000);
        pause_ = qRound(poi->pause * 1000);
    }

    virtual ~OGCamera() {}

    QPoint center() const { return center_; }
    QSize size() const { return size_; }
    float zoom() const { return zoom_; }
    int traveltime() const { return traveltime_; }
    int pause() const { return pause_; }

    void SetCenter(const QPoint & center) { center_ = center; }
    void SetCenter(float x, float y) { center_ = QPoint(x, y); }
    void SetSize(const QSize & size) { size_ = size; }
    void SetTraveltime(int traveltime) { traveltime_ = traveltime; }
    void SetPause(int pause) { pause_ = pause; }
    void SetZoom(float zoom) { zoom_ = zoom; }
};

#endif // OG_CAMERA_H
