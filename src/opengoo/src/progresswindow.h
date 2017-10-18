#pragma once

#include <QObject>
#include <memory>
#include <OGLabel>
#include "continuebutton.h"


class ProgressWindow : public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY(ProgressWindow)

public:
    ProgressWindow();
    ~ProgressWindow();

    void setBalls(int balls, int extraBalls);

signals:
    void close();

private:
    struct Impl;
    std::unique_ptr<Impl> _pImpl;
};
