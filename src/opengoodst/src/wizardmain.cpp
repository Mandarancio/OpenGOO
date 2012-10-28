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

    } else return;
}

WizardMain::WizardMain(Report *report, XmlModule *xml, bool mode, QWidget *parent) : //!Launched by ManagerGui.
    QWizard(parent)
{
    //!
    //!mode boolean var indicates if the report is pure textual or a normal report.
    //!If mode is set to true, the wizard is launched with the wizard_s2 page.
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

    //setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);          //The titlebar is customized to delete the close button. NOT USED, DECENTRALIZES THE WIZARD ON THE SCREEN.

    //If false, the wizard will be launch from s2 page.
    if(mode==false) {

        //The first wizard page is visible only if launched from console in wizard mode,
        //parent is NULL only if the wizard is launch by managergui class.

        if(parent == NULL)
            addPage(new WizardGui_s0);

        addPage(new WizardGui_s1);
    }

    addPage(new WizardGui_s2);
    addPage(new WizardGui_s3);

    if(mode == false) {
        connect(page(2), SIGNAL(reportSent()),this, SLOT(wizardReportSent())); //A pointer to the WizardGui_s2 is set, the class will emit the reportSent() SIGNAL when the report is sent and should be saved on file.
    }
    else {
        connect(page(0), SIGNAL(reportSent()),this, SLOT(wizardReportSent())); //A pointer to the WizardGui_s2 is set, the class will emit the reportSent() SIGNAL when the report is sent and should be saved on file.
    }

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

    //It saves the changes only if there are edits from the user.

    QString userText;

    if(mode == false) {

        userText = this->field("UserDescription").toString();
    } else {

        //If the user closes the wizard the textual report is saved.
        xml->save();
    }

    if (userText.isEmpty() == false && QString::compare(report->getUserText(), userText) != 0) {

        report->setUserText(userText); //User description inserted.
        xml->save(); //The changes are saved on file when the user has changed the description.
    }

    if(currentId() == 3)
    event->ignore();

    emit rejected(); //The signal is used by sendtextreportform class to close the form.
}

bool WizardMain::getMode() {
    //!
    //!It returns the actual mode of the wizard (only textual or complete)
    //!

    return mode;
}
