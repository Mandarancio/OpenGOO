#include "og_windowcamera.h"
#include <QDebug>

OGWindowCamera::OGWindowCamera(const OGCamera & camera)
{
    OGCamera(camera.center(), camera.size(), camera.zoom()
             , camera.traveltime(), camera.pause());

    Scale(zoom());
}

OGWindowCamera& OGWindowCamera::operator=(const OGCamera & camera)
{
    center(camera.center());
    size(camera.size());
    zoom(camera.zoom());
    traveltime(camera.traveltime());
    pause(camera.pause());
    Scale(zoom());

    return *this;
}

void OGWindowCamera::SetPosition(const QPointF &position)
{
    center(position);
    window_.moveCenter(ToWindowCoordinates_(center()));
}

void OGWindowCamera::SetPosition(qreal x, qreal y)
{
    center(x, y);
    window_.moveCenter(ToWindowCoordinates_(center()));
}

void OGWindowCamera::SetX(qreal x)
{
    center(x, center().y());
    window_.moveCenter(ToWindowCoordinates_(center()));
}

void OGWindowCamera::SetY(qreal y)
{
    center(center().x(), y);
    window_.moveCenter(ToWindowCoordinates_(center()));
}

QPointF OGWindowCamera::ToWindowCoordinates_(const QPointF &center)
{
    return QPointF(center.x(), center.y()*(-1.0));
}

void OGWindowCamera::Scale(qreal zoom)
{
    OGCamera::zoom(zoom);
    qreal w = size().width()/zoom;
    qreal h = size().height()/zoom;
    window_.setSize(QSizeF(w, h));
    window_.moveCenter(ToWindowCoordinates_(center()));
}
