#ifndef RECTF_H
#define RECTF_H

#include "rect.h"
#include "util.h"

namespace oglib
{
class RectF
{
        float maxX_;
        float maxY_;
        float minX_;
        float minY_;

    public:
        RectF(float maxx, float maxy, float minx, float miny)
            : maxX_(maxx), maxY_(maxy), minX_(minx), minY_(miny) {}

        float maxx() const { return maxX_; }
        float maxy() const { return maxY_; }
        float minx() const { return minX_; }
        float miny() const { return minY_; }

        Rect ToRect()
        {
            int maxX = RoundF(maxX_);
            int maxY = RoundF(maxY_);
            int minX = RoundF(minX_);
            int minY = RoundF(minY_);

            return Rect(maxX, maxY, minX, minY);
        }
};
}

#endif // RECTF_H
