#include "wizardgui_s2.h"
#include "ui_wizardgui_s2.h"

WizardGui_s2::WizardGui_s2(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::WizardGui_s2)
{
    ui->setupUi(this);
}

WizardGui_s2::~WizardGui_s2()
{
  delete ui;
}

void WizardGui_s2::showEvent(QShowEvent *) {
    //!
    //!The function generates on wizardpage startup the report to be sent, if the wizard is in textual mode, the user description
    //!has been inserted in sendtextreportform class.
    //!

    wizard()->setButtonText(QWizard::NextButton, tr("&Send!"));
    wizard()->setButtonText(QWizard::BackButton, tr("&Back"));

    //Generating the textual report::

    QString userText;

    WizardMain* wizardPointer= dynamic_cast<WizardMain*>(wizard());
    reportPointer = wizardPointer->getReport();

    if(wizardPointer->getMode() == false) {

        //Normal Report.
            userText = wizard()->field("UserDescription").toString();   //User description inserted previously.
    } else {

        //Textual report.
            userText = reportPointer->getUserText();
            wizard()->button(QWizard::CancelButton)->setVisible(false); //Disables the cancel button.
    }

    reportPointer->setUserText(userText);

    ui->bugReport->setText(reportPointer->getTextualDescription());
}

bool WizardGui_s2::validatePage() {
    //!
    //!The function is called automatically before the wizard can open the last page as soon as the user press the send button.
    //!Here the report is sent and when all is done the function must return true to go further.
    //!

    //The buttons are disabled:
    wizard()->button(QWizard::NextButton)->setDisabled(true);
    wizard()->button(QWizard::BackButton)->setDisabled(true);

    uploadState = false;

    //An unblocking dialog is shown:

    waitMsg = new QMessageBox(this);
    waitMsg->setAttribute( Qt::WA_DeleteOnClose );
    waitMsg->setText("Wait...uploading");
    waitMsg->setStandardButtons(QMessageBox::NoButton);
    waitMsg->setModal( true );
    waitMsg->show();                                        //Non blocking function.

    //Network initialisation:

    QFtp *ftp = new QFtp(this);

    connect(ftp, SIGNAL(commandFinished(int,bool)),this, SLOT(ftpReply(int,bool)));
    connect(ftp, SIGNAL(stateChanged(int)), this, SLOT(ftpState(int)));
    connect(ftp, SIGNAL(done(bool)), this, SLOT(ftpEndConnection(bool)));

    qWarning() << "Trasmission mode = " << ftp->setTransferMode(QFtp::Active);
    qWarning() << "Host connection = " << ftp->connectToHost("HOST");
    ftpLoginCommandId = ftp->login("USERNAME", "PASSWORD");

    //Report upload:

    //TODO: The name of the file should include the quuid generated.
    ftpPutCommandId = ftp->put(QByteArray().append(reportPointer->getTextualDescription()), "id.report", QFtp::Ascii);

    qWarning() << "Close connection = " << ftp->close();

    freezeLoop.exec();                                     //The function is blocked here. Will be unlocked in ftpEndConnection.
                                                           //The qftp functions are asynchronous.
    delete ftp;

    return uploadState;
}

void WizardGui_s2::ftpReply(int id, bool error) {

    //TODO: CHECK THIS FUNCTION.

    qWarning() << id << "," << error;

    //If the command just finished is 'login', the next will be 'put', here the send date of the report is set.

    if (id == ftpLoginCommandId && error != true)
        reportPointer->setSendDate(QDateTime::currentDateTimeUtc());    //The send date is set.


    //If the put command generates error the send date is reset blank.

    if(id == ftpPutCommandId && error == true) {
        reportPointer->setSendDate(QDateTime());
    }
}

void WizardGui_s2::ftpState(int dato) {
    //!
    //!The function detects when the ftp connection changes its state.
    //!

    qWarning() << "stato: " << dato;
}

void WizardGui_s2::ftpEndConnection(bool error) {
    //!
    //!When the ftp->close(); function is called the slot is executed.
    //!Here the waiting dialog is closed and, if there isn't any error, the uploadState var assumes true and the freezeLoop is unlocked.
    //!This will help validatePage() return true or false in case of error or not.
    //!

    waitMsg->accept();                              //The waiting dialog is closed.

    if(!error) {
        reportPointer->setSent(true);               //The report has been sent correctly, it is marked like sent.
        emit reportSent();                          //and the signal is emitted. It will call wizardReportSent() in WizardMain.

        uploadState=true;
    } else {
        //Error message:

        QMessageBox msgBox;
        msgBox.setText("Some errors are occurred! \n\nProbably no internet connection is active, \njust close the wizard to send the report later.");
        msgBox.exec();

        //The buttons are re-enabled:
        wizard()->button(QWizard::NextButton)->setDisabled(false);
        wizard()->button(QWizard::BackButton)->setDisabled(false);

    }

    freezeLoop.exit(0);                             //The loop is unlocked.
}

