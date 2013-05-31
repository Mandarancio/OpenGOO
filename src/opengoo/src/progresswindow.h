#ifndef PROGRESSWINDOW_H
#define PROGRESSWINDOW_H

#include <QObject>

class ProgressWindow : public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY(ProgressWindow)

public:
    ProgressWindow();

    void setBalls(int balls, int extraBalls);

signals:
    void close();

private:
    struct Impl;
    std::unique_ptr<Impl> _pImpl;
};

#endif // PROGRESSWINDOW_H
