#ifndef OG_FPSCOUNTER_H
#define OG_FPSCOUNTER_H

class QPainter;

class OGFPSCounter
{
    int fps_;
    int curFPS_;
    int time_;

public:
    OGFPSCounter();

    void Painter(QPainter* painter);
    void Reset();
    void Update(int time);
};

#endif // OG_FPSCOUNTER_H
