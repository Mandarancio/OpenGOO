#ifndef SENDTEXTREPORTFORM_H
#define SENDTEXTREPORTFORM_H

#include <QWidget>
#include <QDesktopWidget>
#include <QMessageBox>

#include <QDebug>

#include "report.h"
#include "xmlmodule.h"
#include "architecture.h"
#include "managergui.h"

/*!
 *The class provides the form to send the textual reports, reports the user can create that contains only textual description
 *without the backtrace.
 */

namespace Ui {
    class SendTextReportForm;
}

class ManagerGui;

class SendTextReportForm : public QWidget
{
    Q_OBJECT

public:
    explicit SendTextReportForm(Architecture*, XmlModule *xml, ManagerGui *parent = 0);
    ~SendTextReportForm();

private:
    Ui::SendTextReportForm *ui;

    void centerWindow();        //!Center the window on the screen.

    Report *report;             //!The generated report.
    Architecture *archInUse;    //!The arch actually in use by the game.

    ManagerGui *guiPointer;

    XmlModule *xml;             //!Xml module pointer.

signals:
    void reportSent();

private slots:
    void sendTextualReport();
    void emitReportSent();
    void textChanged();         //!Slot that handles the activation of the send button.
};

#endif // SENDTEXTREPORTFORM_H
