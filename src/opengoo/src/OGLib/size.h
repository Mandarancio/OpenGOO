#ifndef SIZE_H
#define SIZE_H

namespace oglib
{
class Size
{
        int width_;
        int height_;

    public:
        Size(int w, int h) : width_(w), height_(h) {}

        int width() const { return width_; }
        int height() const { return height_; }

        void SetWidth(int w) { width_ = w; }
        void SetHeight(int h) { height_ = h; }
};
}

#endif // SIZE_H
