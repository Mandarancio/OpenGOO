#ifndef RETRYMENU_H
#define RETRYMENU_H

#include <memory>
#include <QObject>

class RetryMenu : public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY(RetryMenu)

public:
    RetryMenu();
    
signals:
    void close();    

private:
    struct Impl;
    std::unique_ptr<Impl> _pImpl;

private slots:
    void _ok();
};

#endif // RETRYMENU_H
