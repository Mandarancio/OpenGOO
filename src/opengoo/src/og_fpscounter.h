#ifndef OG_FPSCOUNTER_H
#define OG_FPSCOUNTER_H

#include <QObject>
#include "fpscounter.h"
#include <OGLabel>
#include <memory>

class OGFPSCounter : public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY(OGFPSCounter)

public:
    OGFPSCounter(const QRect &rect);
    ~OGFPSCounter();

    void Reset();
    void Update(int dt);

private:
    struct Impl;
    std::unique_ptr<Impl> _pImpl;

private slots:
    void SetFPS(int fps);
};

#endif // OG_FPSCOUNTER_H
