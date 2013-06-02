#ifndef OG_FPSCOUNTER_H
#define OG_FPSCOUNTER_H

#include <QObject>
#include <memory>

class OGFPSCounter : public QObject
{
    Q_OBJECT

public:
    OGFPSCounter(const QRect &rect);

    void Reset();
    void Update(int dt);

private:
    Q_DISABLE_COPY(OGFPSCounter)

    struct Impl;
    std::unique_ptr<Impl> _pImpl;

private slots:
    void SetFPS(int fps);
};

#endif // OG_FPSCOUNTER_H
