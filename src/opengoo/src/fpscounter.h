#ifndef FPSCOUNTER_H
#define FPSCOUNTER_H

#include <QObject>
#include <memory>

class FPSCounter : public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY(FPSCounter)

public:
    FPSCounter();
    ~FPSCounter();

    int getFPS();

    void reset();
    void update(int dt);

signals:
    void setFPS(int setFPS);

private:
    struct Impl
    {
        int fps;
        int curFPS;
        int time;
    };

    std::unique_ptr<Impl> _pImpl;
};

#endif // FPSCOUNTER_H
