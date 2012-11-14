#include <QDebug>

#include "wizardmain.h"

WizardMain::WizardMain(QUuid uuid, QWidget *parent) : //!Launched by main.
    QWizard(parent)
{
    init(parent);

    //!
    //!Opens the xml file calling the XmlModule and loads the report with same uuid.
    //!

    xml = new XmlModule;

    Report* rep = xml->getByUuid(uuid);

    if(rep != NULL) {

        this->report = rep;

    } else {

        qWarning() << "QUuid not valid";
        return;
    }
}

WizardMain::WizardMain(Report *report, XmlModule *xml, bool mode, QWidget *parent) : //!Launched by ManagerGui.
    QWizard(parent)
{
    //!
    //!mode boolean var indicates if the report is pure textual or a normal report.
    //!If mode is set to true, the wizard is launched with the wizard_s2 page.
    //!
    //!From ManagerGui is launched with false, from sendTextReportForm is launched with true.
    //!

    this->mode = mode;
    init(parent);
    this->xml = xml;
    this->report = report;
}

WizardMain::~WizardMain() {
    delete xml;
}

void WizardMain::init(QWidget *parent) {

    lastPage=0;

    //setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);          //The titlebar is customized to delete the close button. NOT USED, DECENTRALIZES THE WIZARD ON THE SCREEN.

    //If false, the wizard will be launch from s2 page.
    if(mode==false) {

        //The first wizard page is visible only if launched from console in wizard mode,
        //parent is NULL only if the wizard is launched by main.

        if(parent == NULL) {
            addPage(new WizardGui_s0);
            lastPage++;                 //The last page is incremented to identify the last page id.
        }

        addPage(new WizardGui_s1);
        lastPage++;
    }

    addPage(new WizardGui_s2);
    addPage(new WizardGui_s3);

    lastPage++;

    connect(page(lastPage-1), SIGNAL(reportSent()),this, SLOT(wizardReportSent())); //A pointer to the WizardGui_s2 is set, the class will emit the reportSent() SIGNAL when the report is sent and should be saved on file.

    QList<QWizard::WizardButton> layout;
    layout << QWizard::Stretch << QWizard::Stretch << QWizard::Stretch << QWizard::Stretch << QWizard::CancelButton << QWizard::BackButton << QWizard::Stretch << QWizard::NextButton << QWizard::FinishButton;
    setOption(QWizard::NoBackButtonOnStartPage, true);
    setOption(QWizard::NoBackButtonOnLastPage, true);
    setOption(QWizard::NoCancelButton);
    setButtonLayout(layout);

    setWindowTitle(tr("OpenGooDebugToolSender"));
    setWindowIcon(QIcon(":/res/images/res/icon.png"));
}

void WizardMain::accept() {
    //!
    //!The wizard is accepted when the user clicks on finished on the last wizardpage.
    //!

    emit reportSent();
    QWizard::accept();
}

Report* WizardMain::getReport() {
    //!
    //!The report pointer is passed between qwizardpages.
    //!

    return report;
}

void WizardMain::wizardReportSent() {
    //!
    //!SLOT: saves the changes of the just sent report, it is called when reportSent() is emitted by WizardGui_s2.
    //!

    xml->save(); //!The changes are saved on file.
}

void WizardMain::closeEvent(QCloseEvent *event) {
    //!
    //!SLOT: catches the close button event on the wizard and ignore it only on the last page.
    //!

    //!If the user closes the wizard the textual report is saved.

    QString userText;

    if(mode == false) {
          userText = this->field("UserDescription").toString();
          report->setUserText(userText); //User description inserted.
    }

    xml->save();

    //The event is ignored on the last page of the wizard because the user must press 'finished'
    //to execute the last code.

    //The last page changes the id according to the wizard mode.

    if(currentId() == lastPage)
        event->ignore();

    emit rejected(); //The signal is used by sendtextreportform class to close the form.
}

bool WizardMain::getMode() {
    //!
    //!It returns the actual mode of the wizard (only textual or complete)
    //!

    return mode;
}
