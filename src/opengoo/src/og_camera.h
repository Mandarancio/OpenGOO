#pragma once

#include "OGLib/icamera.h"
#include "OGLib/point.h"
#include "OGLib/size.h"

using namespace oglib;

class OGCamera : public ICamera
{    
protected:
    Point pos_;
    Size size_;
    float zoom_;

public:
    OGCamera(int posx, int posy, int width, int height, float zoom=1)
        : pos_(posx, posy)
        , size_(width, height)
        , zoom_(zoom)
    {
    }

private:
    int _GetX() const { return pos_.x(); }
    int _GetY() const { return pos_.y(); }

    int _GetWidth() const;
    int _GetHeight() const;

    float _GetZoom() const { return zoom_; }

    void _MoveUp(int posy) { pos_.SetY(posy); }
    void _MoveDown(int posy) { pos_.SetY(posy); }
    void _MoveLeft(int posx) { pos_.SetX(posx); }
    void _MoveRight(int posx) { pos_.SetX(posx); }
    void _MoveTo(int posx, int posy) { pos_ = Point(posx, posy); }
    void _MoveTo(float posx, float posy);
    void _MoveTo(const Point& poi) { pos_ = poi; }

    void _SetZoom(float zoom) { zoom_ = zoom; }

    void SetSize(int aWidth, int aHeight)
    {
        size_.Set(aWidth, aHeight);
    }
};
