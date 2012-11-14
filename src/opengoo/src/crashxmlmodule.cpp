#include "crashxmlmodule.h"

#include <QDebug>

CrashXmlModule::CrashXmlModule(QString backtraceText)
{
    //!
    //!Here the report is created and saved on file.
    //!

    XmlModule();

    //Generates the new report.
    newReport = new Report(this->getActualArch(), QUuid::createUuid(), QDateTime().currentDateTimeUtc(), backtraceText, QString(), false, QDateTime(), false, QString());
    this->reports->append(newReport);

    save();
}

QUuid CrashXmlModule::getUuid() {
    //!
    //!The function returns the QUuid of the report generated and saved.
    //!

    return newReport->getUuid();
}
