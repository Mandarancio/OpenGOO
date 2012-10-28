#include "viewreport.h"
#include "ui_viewreport.h"

ViewReport::ViewReport(QString textualDescription, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViewReport)
{
    ui->setupUi(this);
    ui->reportText->setText(textualDescription);

    setWindowTitle(tr("OpenGooDST Report"));
    setWindowIcon(QIcon(":/res/images/res/icon.png"));
    this->centerWindow();
}

ViewReport::~ViewReport()
{
    delete ui;
}

//Center the window on the screen.
void ViewReport::centerWindow() {

    // fixs the size on the window with widget hint.
    this->adjustSize();

    this->setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            this->size(),
            qApp->desktop()->availableGeometry()
      ));
}

