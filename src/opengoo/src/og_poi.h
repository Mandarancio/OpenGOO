#pragma once

#include "OGLib/pointf.h"
#include "wog_level.h"

#include <list>

using namespace oglib;

class OGPoi
{
    public:
        OGPoi(const WOGPoi& poi)
        {
            pos_.Set(poi.position.x(), poi.position.y());
            traveltime_ = poi.traveltime * 1000;
            pause_ = poi.pause * 1000;
            zoom_ = (poi.zoom == 0) ? 1 : 1.0f / poi.zoom;
        }

        float x() const { return pos_.x(); }
        float y() const { return pos_.y(); }
        float zoom() const { return zoom_; }
        float traveltime() const { return traveltime_; }
        float pause() const { return pause_; }

    private:
        PointF pos_;
        float traveltime_;
        float pause_;
        float zoom_;
};

typedef std::list<OGPoi> PoiList;
