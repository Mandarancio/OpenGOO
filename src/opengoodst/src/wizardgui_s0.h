#ifndef WIZARDGUI_S0_H
#define WIZARDGUI_S0_H

#include <QWizardPage>

namespace Ui {
    class WizardGui_s0;
}

class WizardGui_s0 : public QWizardPage
{
    Q_OBJECT

public:
    explicit WizardGui_s0(QWidget *parent = 0);
    ~WizardGui_s0();

private:
    Ui::WizardGui_s0 *ui;
    void showEvent(QShowEvent *); //Override.
};

#endif // WIZARDGUI_S0_H
