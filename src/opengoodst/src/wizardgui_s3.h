#ifndef WIZARDGUI_S3_H
#define WIZARDGUI_S3_H

#include <QWizardPage>

namespace Ui {
    class WizardGui_s3;
}

class WizardGui_s3 : public QWizardPage
{
    Q_OBJECT

public:
    explicit WizardGui_s3(QWidget *parent = 0);
    ~WizardGui_s3();

private:
    Ui::WizardGui_s3 *ui;
    void showEvent(QShowEvent *); //Override.
};

#endif // WIZARDGUI_S3_H
