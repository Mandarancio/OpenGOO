#include "managergui.h"
#include "ui_managergui.h"

ManagerGui::ManagerGui(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ManagerGui)
{
    ui->setupUi(this);

    //Tables init:

    //Those lines adapt the vertical headers within the content leaving users resize capabilities.
    ui->sentTable->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
    ui->notSentTable->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);

    //Window init:

    this->centerWindow();
    setWindowTitle(tr("OpenGooDebugSenderTool Manager"));
    setWindowIcon(QIcon(":/res/images/res/icon.png"));

    startUp();

    //TODO: Check for new updates downloading the xml file with solved uuid.
}

void ManagerGui::centerWindow() {
    //!
    //!Centers the window on the screen.

    // fixes the size on the window with widget hint.
    this->adjustSize();

    this->setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            this->size(),
            qApp->desktop()->availableGeometry()
      ));
}

ManagerGui::~ManagerGui()
{
    delete ui;
}

void ManagerGui::sendReport() {
    //!
    //!SLOT: gets the selected report from the not sent tableview and passes it to the wizard.
    //!

    QModelIndexList list = ui->notSentTable->selectionModel()->selectedIndexes();
    QModelIndex index;

    if(!list.isEmpty())
        index = list.at(0);

    if(index.isValid()) {

        WizardMain *w = new WizardMain(model->getReport( notSentProxy->mapToSource(index)), xml, false, this);
        w->setModal(true);

        connect(w, SIGNAL(reportSent()),this, SLOT(reportSent()));

        w->show();

    } else {
        QMessageBox::information(this, tr("OpenGooDST"),
                                        tr("Select one report to be sent\n"
                                           "from the not sent ones."));
    }
}

void ManagerGui::deleteReport() {
    //!
    //!SLOT: gets the selected report from the not sent tableview and deletes it.
    //!

    QModelIndexList list = ui->notSentTable->selectionModel()->selectedIndexes();
    QModelIndex index;

    if(!list.isEmpty()) {
        index = list.at(0);

        if(index.isValid()) {

            int ret = QMessageBox::question(this, tr("My OpenGooDST"),
                                            tr("Delete the report.\n"
                                               "Are you sure you want to delete the report?"),
                                            QMessageBox::Yes | QMessageBox::Cancel );

            if (ret == QMessageBox::Yes) {

                QModelIndex delReportIndex = notSentProxy->mapToSource(index);
                model->removeRows(delReportIndex.row(), 1, delReportIndex);

                //Then the changes are saved:
                xml->save();

            }
        }

    }
}

void ManagerGui::sendTextualReport() {
    //!
    //!Shows the textual report form.
    //!

    SendTextReportForm *sf = new SendTextReportForm(xml->getActualArch(), xml, this);
    connect(sf, SIGNAL(reportSent()),this, SLOT(reportSent()));

    sf->show();
}

void ManagerGui::startUp() {
    //!
    //!Starting function.
    //!

    xml = new XmlModule;                            //Interface to xml file.

    QList<Report*> *reports = xml->getReports();    //Retrieve a pointer list to the reports.
    model = new TablesModel(reports);               //Init table model.

    notSentProxy = new CustomTableProxy(false);
    sentProxy = new CustomTableProxy(true);

    notSentProxy->setDynamicSortFilter(true);
    sentProxy->setDynamicSortFilter(true);

    sentProxy->setSourceModel(model);
    notSentProxy->setSourceModel(model);

    ui->notSentTable->setModel(notSentProxy);
    ui->sentTable->setModel(sentProxy);
}

void ManagerGui::notSentDoubleClicked ( const QModelIndex & index ) {
    //!
    //!SLOT: Catches the double clicks from the notSent table.
    //!

    QModelIndex realIndex = notSentProxy->mapToSource(index);
    Report *selectedReport = model->getReport(realIndex);

    ViewReport *viewReport = new ViewReport(selectedReport->getTextualDescription());
    viewReport->show();
}

void ManagerGui::sentDoubleClicked ( const QModelIndex & index ) {
    //!
    //!SLOT: Catches the double clicks from the sent table.
    //!

    QModelIndex realIndex = sentProxy->mapToSource(index);
    Report *selectedReport = model->getReport(realIndex);

    ViewReport *viewReport = new ViewReport(selectedReport->getTextualDescription());
    viewReport->show();
}

TablesModel* ManagerGui::getDataModel() {
    return model;
}

void ManagerGui::reportSent() {
    //!
    //!SLOT: Called when the wizard is closed emitting reportSent() SIGNAL, it invalidates the proxy filter updating the tableviews.
    //!

    //Invalidation of proxies filter for tableview update:
    notSentProxy->invalidate();
    sentProxy->invalidate();
}




