#include "wizardgui_s1.h"
#include "ui_wizardgui_s1.h"

#include <QDebug>

WizardGui_s1::WizardGui_s1(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::WizardGui_s1)
{
    ui->setupUi(this);
    registerField("UserDescription", ui->userText, "plainText");
}

WizardGui_s1::~WizardGui_s1()
{
    delete ui;
}

void WizardGui_s1::showEvent(QShowEvent *) {

    wizard()->setOption(QWizard::NoCancelButton, true);
    wizard()->setButtonText(QWizard::NextButton, tr("&Summary"));
    wizard()->button(QWizard::CancelButton)->setVisible(false);
    wizard()->button(QWizard::BackButton)->setVisible(false);

    WizardMain* wizardPointer= dynamic_cast<WizardMain*>(wizard());
    ui->userText->setText(wizardPointer->getReport()->getUserText());
}
