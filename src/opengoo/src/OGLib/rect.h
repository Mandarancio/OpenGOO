#ifndef RECT_H
#define RECT_H

namespace oglib
{
class Rect
{
        int maxX_;
        int maxY_;
        int minX_;
        int minY_;

    public:
        Rect(int maxx, int maxy, int minx, int miny)
            : maxX_(maxx), maxY_(maxy), minX_(minx), minY_(miny) {}

        int maxx() const { return maxX_; }
        int maxy() const { return maxY_; }
        int minx() const { return minX_; }
        int miny() const { return minY_; }
};
}
#endif // OG_RECT_H
