#include "og_windowcamera.h"
#include <QDebug>

OGWindowCamera::OGWindowCamera(const OGCamera& camera)
    : OGCamera(camera.center(), camera.size(), camera.zoom()
               , camera.traveltime(), camera.pause())
{     
    Zoom(zoom());
}

OGWindowCamera& OGWindowCamera::operator=(const OGCamera & camera)
{
    SetCenter(camera.center());
    SetSize(camera.size());
    SetZoom(camera.zoom());
    SetTraveltime(camera.traveltime());
    SetPause(camera.pause());
    Zoom(zoom());

    return *this;
}

void OGWindowCamera::SetPosition(const QPoint &position)
{
    SetCenter(position);
    window_.moveCenter(_ToWindowCoordinates(center()));
}

void OGWindowCamera::SetPosition(int x, int y)
{
    SetCenter(x, y);
    window_.moveCenter(_ToWindowCoordinates(center()));
}

void OGWindowCamera::SetX(int x)
{
    SetCenter(x, center().y());
    window_.moveCenter(_ToWindowCoordinates(center()));
}

void OGWindowCamera::SetY(int y)
{
    SetCenter(center().x(), y);
    window_.moveCenter(_ToWindowCoordinates(center()));
}

QPoint OGWindowCamera::_ToWindowCoordinates(const QPoint &center)
{
    return QPoint(center.x(), center.y()*(-1));
}

void OGWindowCamera::Zoom(float zoom)
{
    if (zoom == 0) return;

    SetZoom(zoom);
    int w = qRound(size().width() / zoom);
    int h = qRound(size().height() / zoom);
    window_.setSize(QSize(w, h));
    window_.moveCenter(_ToWindowCoordinates(center()));
}
