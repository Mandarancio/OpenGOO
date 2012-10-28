#include "wizardgui_s0.h"
#include "ui_wizardgui_s0.h"

WizardGui_s0::WizardGui_s0(QWidget *parent) :
    QWizardPage(parent),
    ui(new Ui::WizardGui_s0)
{
    ui->setupUi(this);
    setPixmap(QWizard::WatermarkPixmap, QPixmap((":/res/images/res/black-goo.riscaled.png")));
}

WizardGui_s0::~WizardGui_s0()
{
    delete ui;
}

void WizardGui_s0::showEvent(QShowEvent *) {

    wizard()->setButtonText(QWizard::NextButton, tr("&Next"));
    wizard()->setButtonText(QWizard::CancelButton, tr("&No"));
}
