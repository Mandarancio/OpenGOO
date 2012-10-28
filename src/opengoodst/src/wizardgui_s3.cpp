#include "wizardgui_s3.h"
#include "ui_wizardgui_s3.h"

WizardGui_s3::WizardGui_s3(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::WizardGui_s3)
{
    ui->setupUi(this);
    setPixmap(QWizard::WatermarkPixmap, QPixmap((":/res/images/res/black-goo.riscaled.png")));
}

WizardGui_s3::~WizardGui_s3()
{
    delete ui;
}

void WizardGui_s3::showEvent(QShowEvent *) {

    wizard()->setButtonText(QWizard::FinishButton, tr("&Finish"));
}
