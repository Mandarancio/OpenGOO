#pragma once

#include "OGLib/pointf.h"
#include "wog_level.h"

#include <vector>

using namespace oglib;

class OGPoi
{
    public:
        OGPoi(const WOGPoi& poi)
        {
            pos_.SetX(poi.position.x());
            pos_.SetY(poi.position.y());
            traveltime_ = poi.traveltime * 1000;
            pause_ = poi.pause * 1000;
            zoom_ = poi.zoom;
            if (zoom_ == 0) zoom_ = 1;
            zoom_ = 1 / zoom_;
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

typedef std::vector<OGPoi> PoiList;
