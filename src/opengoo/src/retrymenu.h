#pragma once

#include <QObject>

class RetryMenu : public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY(RetryMenu)

public:
    RetryMenu();
    ~RetryMenu();
    
signals:
    void close();    

private:
    struct Impl;
    std::unique_ptr<Impl> mImpl;

private slots:
    void ok();
};
