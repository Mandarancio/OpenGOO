#ifndef RETRYMENU_H
#define RETRYMENU_H

#include <memory>
#include <QObject>

#include "OGLib/UI/og_ipushbutton.h"

class RetryMenu : public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY(RetryMenu)

public:
    RetryMenu();
    
signals:
    void close();    

private:
    struct Impl
    {
        std::unique_ptr<og::ui::IPushButton> okBtn;
        std::unique_ptr<og::ui::IPushButton> cancelBtn;
    };

    std::unique_ptr<Impl> _pImpl;

private slots:
    void _ok();
};

#endif // RETRYMENU_H
