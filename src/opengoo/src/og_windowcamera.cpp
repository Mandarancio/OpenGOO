#include "og_windowcamera.h"
#include "og_gameengine.h"

using namespace std;
using namespace oglib;

OGWindowCamera* OGWindowCamera::pInstance_ = nullptr;

OGWindowCamera::OGWindowCamera(const Rect &scene, const Size &size, const WOGCamera *cam)
    : target_(0), scene_(scene), traveltime_(0), pause_(0), isScrolling_(true)
{
    pInstance_ = this;

    Q_FOREACH(WOGPoi* poi, cam->poi)
    {
        poiList_.push_back(OGPoi(poi));
    }

    int x = RoundF(poiList_.front().x());
    int y = RoundF(poiList_.front().y());
    float z = poiList_.front().zoom();
    int w = size.width();
    int h = size.height();

    if (z == 0) z = 1;
    else z = 1 / z;

    camera_ = unique_ptr<OGCamera>(new OGCamera(x, y, w, h, z));
}

OGWindowCamera::~OGWindowCamera()
{
    pInstance_ = nullptr;
}

OGWindowCamera* OGWindowCamera::instance()
{
    return pInstance_;
}

void OGWindowCamera::ScrollUp(int shift)
{
    if (!isScrolling_) return;

    int y = camera_->y() + shift;
    int pos = qMin(y, scene_.maxy() - RoundF(camera_->height() * 0.5f));
    camera_->MoveUp(pos);
}

void OGWindowCamera::ScrollDown(int shift)
{
    if (!isScrolling_) return;

    int y = camera_->y() - shift;
    int pos = qMax(y, scene_.miny() + RoundF(camera_->height() * 0.5f));
    camera_->MoveDown(pos);
}

void OGWindowCamera::ScrollLeft(int shift)
{
    if (!isScrolling_) return;

    int x = camera_->x() - shift;
    int pos = qMax(x, scene_.minx() + RoundF(camera_->width() * 0.5f));
    camera_->MoveLeft(pos);
}

void OGWindowCamera::ScrollRight(int shift)
{
    if (!isScrolling_) return;

    int x = camera_->x() + shift;
    int pos = qMin(x, scene_.maxx() - RoundF(camera_->width() * 0.5f));
    camera_->MoveRight(pos);
}

QPoint OGWindowCamera::windowToLogical(const QPoint &p)
{
    auto px = p.x();
    auto py = p.y();

    if (!GE->isCrt())
    {
        auto window = GE->getWindow();
        px *= camera_->width() / (float)window->width();
        py *= camera_->height() / (float)window->height();
    }

    int a = RoundF(px * camera_->zoom() - camera_->width() * 0.5f);
    int b = RoundF(py * camera_->zoom() - camera_->height() * 0.5f);
    int x = camera_->x() + a;
    int y = camera_->y() - b;

    return QPoint(x, y);
}

QRect OGWindowCamera::rect() const
{
    int x = camera_->x() - RoundF(camera_->width() * 0.5f);
    int y = (camera_->y() + RoundF(camera_->height() * 0.5f)) * -1;
    return QRect(x, y, camera_->width(), camera_->height());
}

void OGWindowCamera::Update(int time)
{                
    if (target_ == 0 && poiList_.size() > 1)
    {
        traveltime_ = 0;
        pause_ = 0;
        isScrolling_ = false;
        float x = poiList_.front().x();
        float y = poiList_.front().y();
        initialPosition_ = Vector2D(x, y);
        _SetTarget();

        if (target_->pause() > 0) pause_ = time;
    }

    if (target_ == 0) return;

    if (pause_ < target_->pause())
    {
        pause_ += time;
        return;
    }

    traveltime_ += time;

    if (traveltime_ < target_->traveltime())
    {
        Vector2D pos = initialPosition_ + velocity_ * traveltime_;
        camera_->MoveTo(pos.ToPoint());
        float z = poiList_.front().zoom() + zoom_ * traveltime_;
        camera_->SetZoom(z);
    }
    else
    {
        camera_->MoveTo(target_->x(), target_->y());
        target_ = 0;
        poiList_.erase(poiList_.begin());        
    }

    if (poiList_.size() == 1) isScrolling_ = true;
}

void OGWindowCamera::_SetTarget()
{
    target_ = &poiList_.at(1);
    finalPosition_ = Vector2D(target_->x(), target_->y());
    float time = target_->traveltime();

    if (time == 0) velocity_.SetZero();
    else velocity_ = (finalPosition_ - initialPosition_) / time;

    zoom_ = (target_->zoom() - poiList_.front().zoom()) / time;
}

void OGWindowCamera::SetLastPosition()
{
    OGPoi& last = poiList_.back();
    camera_->MoveTo(last.x(), last.y());
}
