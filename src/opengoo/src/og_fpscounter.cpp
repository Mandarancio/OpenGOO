#include "og_fpscounter.h"

using namespace og::ui;

struct OGFPSCounter::Impl
{
    FPSCounter counter;
    Label label;
};

OGFPSCounter::OGFPSCounter(const QRect &rect) : _pImpl(new Impl)
{
    connect(&_pImpl->counter, SIGNAL(setFPS(int)), this, SLOT(SetFPS(int)));

    int w = rect.width();
    int h = rect.height();
    int x = rect.x();
    int y = rect.y();
    _pImpl->label.setSize(w, h);
    _pImpl->label.setPosition(x, y);
    _pImpl->label.setVisible(true);
}

OGFPSCounter::~OGFPSCounter() {}

void OGFPSCounter::Reset() { _pImpl->counter.reset(); }

void OGFPSCounter::Update(int dt) { _pImpl->counter.update(dt); }

void OGFPSCounter::SetFPS(int fps)
{
    _pImpl->label.setText(QString::number(fps));
}
