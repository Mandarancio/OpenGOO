#ifndef XMLMODULE_H
#define XMLMODULE_H

#include <QDomNode>
#include <QDomDocument>
#include <QFile>
#include <QList>
#include <QDebug>

#include "architecture.h"
#include "report.h"

class XmlModule
{

public:
    XmlModule();

    QList<Report*>* getReports();
    Architecture* getActualArch();
    Report* getByUuid(QUuid);

    bool save();

private:

    QDomDocument *doc;
    QList<Architecture*> archs;
    QList<Report*> *reports;

    void load();

    //Save-support functions:
    void createXmlArchitecture(QDomElement*, Architecture*);
    void createXmlReport(QDomElement*, Report*);
};

#endif // XMLMODULE_H
