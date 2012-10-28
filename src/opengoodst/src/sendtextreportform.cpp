#include "sendtextreportform.h"
#include "ui_sendtextreportform.h"

SendTextReportForm::SendTextReportForm(Architecture* arch, XmlModule *xml, ManagerGui *guiPointer) :
    QWidget(),
    ui(new Ui::SendTextReportForm)
{
    ui->setupUi(this);

    this->centerWindow();
    this->guiPointer = guiPointer;
    this->xml = xml;

    ui->sendButton->setEnabled(false);
    connect(ui->userText, SIGNAL(textChanged()), this, SLOT(textChanged()));

    setWindowTitle(tr("OpenGooDebugSenderTool - send notification"));
    setWindowIcon(QIcon(":/res/images/res/icon.png"));

    archInUse = arch;
}

SendTextReportForm::~SendTextReportForm()
{
    delete ui;
}

void SendTextReportForm::centerWindow() {
    //Centers the window on the screen.

    //Fixes the size on the window with widget hint.
    this->adjustSize();

    this->setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            this->size(),
            qApp->desktop()->availableGeometry()
      ));
}

void SendTextReportForm::sendTextualReport() {
    //!
    //!The function asks the user to send the textual report and use part of the wizard code to send the report.
    //!

    int ret = QMessageBox::question(this, tr("My OpenGooDST"),
                                    tr("Send the report.\n"
                                       "Are you sure you want to send the report?"),
                                    QMessageBox::Yes | QMessageBox::Cancel );

    switch(ret) {

        case QMessageBox::Yes:

            //Generates the new report.
            report = new Report(archInUse, QUuid::createUuid(), QDateTime().currentDateTimeUtc(), QString(), ui->userText->toPlainText(), false, QDateTime(), false, QString());

            //The empty report is inserted into the model.
            guiPointer->getDataModel()->insertRows(report, 0, 1, QModelIndex());

            WizardMain *w = new WizardMain(report, xml, true);
            w->setModal(true);

            connect(w, SIGNAL(reportSent()),this, SLOT(emitReportSent()));
            connect(w, SIGNAL(rejected()), this, SLOT(emitReportSent()));

            w->show();

            break;

//        case QMessageBox::Cancel:
//            break;
    }

}

void SendTextReportForm::emitReportSent() {
    emit close();
    emit reportSent();
}

void SendTextReportForm::textChanged() {

    if (ui->userText->toPlainText().isEmpty() == false) {
        ui->sendButton->setEnabled(true);
    } else {
        ui->sendButton->setEnabled(false);
    }
}
