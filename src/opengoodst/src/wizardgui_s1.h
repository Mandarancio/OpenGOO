#ifndef WIZARDGUI_S1_H
#define WIZARDGUI_S1_H

#include <QWizardPage>
#include <QAbstractButton>

#include "wizardmain.h"

namespace Ui {
    class WizardGui_s1;
}

class WizardGui_s1 : public QWizardPage
{
    Q_OBJECT

public:
    explicit WizardGui_s1(QWidget *parent = 0);
    ~WizardGui_s1();

private:
    Ui::WizardGui_s1 *ui;
    void showEvent(QShowEvent *); //Override.
};

#endif // WIZARDGUI_S1_H
