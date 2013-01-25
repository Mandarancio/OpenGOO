#ifndef OG_CAMERA_H
#define OG_CAMERA_H

#include <QPoint>
#include <QRect>

class OGCamera
{
    QPointF center_;
    QSizeF size_;
    qreal zoom_;
    qreal traveltime_;
    qreal pause_;        

public:
    OGCamera() : center_(QPointF()), size_(QSizeF()), zoom_(0), traveltime_(0)
      , pause_(0) { }

    OGCamera(const QPointF & center, const QSizeF & size, qreal zoom
           , qreal traveltime, qreal pause)
        : center_(center), size_(size), zoom_(zoom), traveltime_(traveltime)
        , pause_(pause) {}

    virtual ~OGCamera() {}

    QPointF center() const { return center_; }
    QSizeF size() const { return size_; }
    qreal zoom() const { return zoom_; }
    qreal traveltime() const { return traveltime_; }
    qreal pause() const { return pause_; }

    void center(const QPointF & center) { center_ = center; }
    void center(qreal x, qreal y) { center_ = QPointF(x, y); }
    void size(const QSizeF & size) { size_ = size; }
    void traveltime(qreal traveltime) { traveltime_ = traveltime; }
    void pause(qreal pause) { pause_ = pause; }
    void zoom(qreal zoom) { zoom_ = zoom; }
};

#endif // OG_CAMERA_H
