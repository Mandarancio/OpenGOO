#ifndef WIZARDMAIN_H
#define WIZARDMAIN_H

#include <QDesktopWidget>
#include <QCloseEvent>
#include <QWizard>
#include <QUuid>

#include "wizardgui_s0.h"
#include "wizardgui_s1.h"
#include "wizardgui_s2.h"
#include "wizardgui_s3.h"

#include "report.h"
#include "xmlmodule.h"

class WizardMain : public QWizard
{
    Q_OBJECT

private:
    void closeEvent(QCloseEvent *); //Override.
    void accept();  //Override.
    void init(QWidget *parent);

    Report *report;
    XmlModule *xml;
    bool mode;                      //!Mode boolean var indicates if the report is pure textual or a normal report.

    int lastPage;                   //!Indicates the last page id of the wizard.

public:
    explicit WizardMain(QUuid uuid, QWidget *parent = 0);                                   //!Launched by main (wizard mode).
    explicit WizardMain(Report *report, XmlModule *xml, bool mode, QWidget *parent = 0);    //!Launched by ManagerGui (manager mode).
    ~WizardMain();

    Report *getReport();
    bool getMode();

signals:
    void reportSent();

private slots:
    void wizardReportSent();

};

#endif // WIZARDMAIN_H
