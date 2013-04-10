#include "og_fpscounter.h"

#include <QPainter>

OGFPSCounter::OGFPSCounter()
{
    Reset();
}

void OGFPSCounter::Reset()
{
    time_ = 0;
    curFPS_ = 0;
    fps_ = 0;
}

void OGFPSCounter::Update(int time)
{
    curFPS_++;
    time_ += time;

    if (time_ >= 1000)
    {
        fps_ = curFPS_;
        curFPS_ = 0;
        time_ = 0;
    }
}

void OGFPSCounter::Painter(QPainter *painter)
{
    painter->save();
    painter->setPen(Qt::white);
    painter->setFont(QFont("Verdana", 12.0, QFont::Bold));
    painter->drawText(20, 20, QString::number(fps_));
    painter->restore();
}
