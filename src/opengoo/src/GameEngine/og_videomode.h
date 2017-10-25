#pragma once

class OGVideoMode
{
public:
    OGVideoMode()
        : width_(0)
        , height_(0)
        , bpp_(0)
        , frequency_(0) {}
    OGVideoMode(int width, int height, int bpp, int frequency)
        : width_(width)
        , height_(height)
        , bpp_(bpp)
        , frequency_(frequency) {}

    static OGVideoMode getCurrentMode();
    static bool returnDefaultMode();
    static bool setVideoMode(int width, int height);
    static bool setVideoMode(const OGVideoMode& mode);
    static bool testVideoMode(int width, int height);

    int width() const { return width_; }
    int height() const { return height_; }
    int bpp() const { return bpp_; }
    int frequency() const { return frequency_; }

private:
    int width_;
    int height_;
    int bpp_;
    int frequency_;
};
