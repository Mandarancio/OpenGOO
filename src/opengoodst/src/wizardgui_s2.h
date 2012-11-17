#ifndef WIZARDGUI_S2_H
#define WIZARDGUI_S2_H

#include <QWizardPage>
#include <QMessageBox>
#include <QEventLoop>
#include <QDebug>
#include <QFtp>


#include "wizardmain.h"
#include "report.h"

namespace Ui {
    class WizardGui_s2;
}

class WizardGui_s2 : public QWizardPage
{
    Q_OBJECT

public:
    explicit WizardGui_s2(QWidget *parent = 0);
    ~WizardGui_s2();

private:
    Ui::WizardGui_s2 *ui;
    void showEvent(QShowEvent *); //Override.
    bool validatePage();          //Override.

    Report *reportPointer;        //!The pointer to the report to upload.
    QMessageBox *waitMsg;         //!A waiting message without buttons showed during the upload.
    bool uploadState;             //!Flags if the upload has finished correctly and let the wizard go further.
    QEventLoop freezeLoop;        //!It freezes the validatePage() execution cause all qftp calls are asynchronous.

    int ftpLoginCommandId;        //!Numeric id of the ftp command. It's used from qftp function to detect the command.
    int ftpPutCommandId;          //!Numeric id of the ftp command. It's used from qftp function to detect the command.

signals:
    void reportSent();             //!The signal is emitted when the report has been sent.

private slots:
    void ftpReply(int id, bool error);
    void ftpState(int);
    void ftpEndConnection(bool);
};

#endif // WIZARDGUI_S2_H
