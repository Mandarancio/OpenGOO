#ifndef VIEWREPORT_H
#define VIEWREPORT_H

#include <QWidget>
#include <QDesktopWidget>

namespace Ui {
    class ViewReport;
}

class ViewReport : public QWidget
{
    Q_OBJECT

public:
    explicit ViewReport(QString textualDescription, QWidget *parent = 0);
    ~ViewReport();

private:
    Ui::ViewReport *ui;

    void centerWindow();    //Center the window on the screen.
};

#endif // VIEWREPORT_H
