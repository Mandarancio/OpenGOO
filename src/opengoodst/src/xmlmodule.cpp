#include "xmlmodule.h"

const QString XMLPATH="./resources/test.xml";

XmlModule::XmlModule()
{

    reports = new QList<Report*>;
    doc = new QDomDocument( "debugFile" );

    load();
}

void XmlModule::load() {
    //!
    //!Opens xml file and loads reports in memory.
    //!

    QFile file( XMLPATH );
    if( !file.open( QIODevice::ReadOnly ) )
        return;

    if( !doc->setContent( &file ) )
    {
      file.close();
      return;
    }
    file.close();

    QDomElement root = doc->documentElement();
    if( root.tagName() != "gameReports" )
    return;

    QList <QDomElement> sysInfos; //!Contains all sysInfos sections - support qlist.

    QDomNode n = root.firstChild();
    while( !n.isNull() )
    {

        QDomElement e = n.toElement();
        if( !e.isNull() )
        {
          if( e.tagName() == "sysInfo" )
          {
              sysInfos << e;
          }
        }

        n = n.nextSibling();
    }

    //Generates list of Architecture-s:

    for(int i=0; i<sysInfos.count(); i++) {

        QDomElement app = sysInfos.at(i);

        QString os = app.attribute("os");
        QString build = app.attribute("build");
        QString gameVer = app.attribute("gameVer");
        QString archText = app.attribute("arch");
        QString driverVer = app.attribute("driverVer");
        QString vga = app.attribute("vga");
        bool inUse = QVariant(app.attribute("inUse")).toBool();

        if(!(os.isEmpty() && build.isEmpty() && gameVer.isEmpty() && archText.isEmpty() && driverVer.isEmpty() && vga.isEmpty())) {

//            qWarning() << "XmlModule" << gameVer;
            Architecture *arch = new Architecture(os, build, gameVer, archText, driverVer, vga, inUse);
            archs.append(arch);

            //Generates list of related Report-s:

            QDomNode reportNode = app.firstChild();
            while(!reportNode.isNull()) {

                QDomElement reportElement = reportNode.toElement();
                if( !reportElement.isNull() ) {
                    if( reportElement.tagName() == "report" ) {

                        QString uuid = reportElement.attribute("uuid");
                        QString dateTime = reportElement.attribute("date-time");
                        QString debugOutput = reportElement.attribute("debugOutput");
                        QString userDescription = reportElement.attribute("userDescription");
                        QString sendDate = reportElement.attribute("sendDate");
                        QString gameVerFix = reportElement.attribute("gameVerFix");
                        bool sent = QVariant(reportElement.attribute("sent")).toBool();
                        bool fixed = QVariant(reportElement.attribute("fixed")).toBool();

                        if(!(uuid.isEmpty() && dateTime.isEmpty() && debugOutput.isEmpty() && userDescription.isEmpty() && sendDate.isEmpty())) {

//                            Debug output:
//                            qWarning() << "Os :" << arch->getOs();
//                            qWarning() << uuid << dateTime << debugOutput << userDescription << sent << sendDate;

                            Report *report = new Report(arch, QUuid(uuid), QDateTime().fromString(dateTime), debugOutput, userDescription, QVariant(sent).toBool(), QDateTime().fromString(sendDate), QVariant(fixed).toBool(), QVariant(gameVerFix).toString());
                            reports->append(report);
                        }

                    }
                }

               reportNode = reportNode.nextSibling();
            } //End While cycle.

        } //End if.

    } //End For cycle.

}

QList<Report*>* XmlModule::getReports() {
    return reports;
}

Architecture* XmlModule::getActualArch() {
    //!
    //!Gives the actual architecture in use.
    //!

    for(int i=0; i< archs.size(); i++) {

        if(archs.at(i)->getInUse())
            return archs.at(i);
    }

    return NULL;
}

Report* XmlModule::getByUuid(QUuid uuid) {
    //!
    //!Gives the report with the same uuid passed.
    //!

    for(int i=0; i<reports->size(); i++) {

        if(reports->at(i)->getUuid() == uuid)
            return reports->at(i);
    }

    return NULL;
}

//Save-support functions:
void XmlModule::createXmlArchitecture(QDomElement * cn, Architecture* arch) {
    //!
    //!Generates the qdomelement from the Achitecture's pointer.
    //!

    //CHECK COMPLETENESS OF FIELDS!!!!!!!!!!!!

    cn->setAttribute( "os", arch->getOs() );
    cn->setAttribute( "arch", arch->getArch() );
    cn->setAttribute( "gameVer", arch->getGameVer() );
    cn->setAttribute( "build", arch->getBuild() );
    cn->setAttribute( "vga", arch->getVga() );
    cn->setAttribute( "driverVer", arch->getDriverVer() );
    cn->setAttribute( "inUse", arch->getInUse() );
}

void XmlModule::createXmlReport(QDomElement* cn, Report* report) {
    //!
    //!Generates the qdomelement from the Report's pointer.
    //!

    //CHECK COMPLETENESS OF FIELDS!!!!!!!!!!!!

    cn->setAttribute( "uuid", report->getUuid().toString() );
    cn->setAttribute( "date-time", report->getDateTime().toString() );
    cn->setAttribute( "debugOutput", report->getDebugText() );
    cn->setAttribute( "userDescription", report->getUserText() );
    cn->setAttribute( "sent", report->getSent() );
    cn->setAttribute( "sendDate", report->getSendDate().toString() );
    cn->setAttribute( "fixed", report->getFixed() );
    cn->setAttribute( "gameVerFix", report->getgameVerFix() );
}

bool XmlModule::save() {
    //!
    //!Saves the changes on file.
    //!

    //!Rebuilds the xml file with the data from the data stuctures.

    QDomDocument doc( "debugFile" );
    QDomElement root = doc.createElement( "gameReports" );
    doc.appendChild( root );

    QList<QDomElement*> ar; //!The Architecture's list.
    QList<QDomElement*> rp; //!The Report's list.

    //Populating the xml document:

    for(int i=0; i< archs.size(); i++) {

        ar.append(new QDomElement);
        *ar.at(ar.size()-1) = doc.createElement( "sysInfo" );

        createXmlArchitecture(ar.at(ar.size()-1), archs.at(i));

        for(int j=0; j<reports->size(); j++) {

            if(archs.at(i) == reports->at(j)->getArchP()) {

                //If the Report is related with this Architecture.

                rp.append(new QDomElement);
                *rp.at(rp.size()-1) = doc.createElement( "report" );

                createXmlReport(rp.at((rp.size()-1)), reports->at(j));

                ar.at(i)->appendChild(*(rp.at(rp.size()-1)));
            }
        }

        root.appendChild(*ar.at(i));
    }

    //Output on file:

    QFile file( XMLPATH );
    if( !file.open( QIODevice::WriteOnly ) )
      return false;

    QTextStream ts( &file );
      ts << doc.toString();

    file.close();

    return true;
}
