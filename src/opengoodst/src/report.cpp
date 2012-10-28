#include "report.h"

Report::Report(Architecture *arch, QUuid uuid, QDateTime dateTime, QString debugText, QString userText, bool sent, QDateTime sendDate, bool fixed, QString gameVerFix)
{
    this->arch = arch;
    this->uuid = uuid;
    this->dateTime = dateTime;
    this->debugText = debugText;
    this->userText = userText;
    this->sent = sent;
    this->sendDate = sendDate;
    this->fixed = fixed;
    this->gameVerFix = gameVerFix;
}

      //-------------------------//
     // -   Setter functions  - //
    //-------------------------//

void Report::setUserText(QString userText) {
    this->userText = userText;
}

void Report::setFixed(bool fixed) {
    this->fixed = fixed;
}

void Report::setSent(bool sent) {
    //!
    //!It sets the sent flag when the report is sent.
    //!

    this->sent = sent;
}

void Report::setSendDate(QDateTime sendDate) {
    //!
    //!It sets the datetime when the report is sent.
    //!

    this->sendDate=sendDate;
}

      //------------------------------//
     // -  Retrive data functions  - //
    //------------------------------//

QString Report::getGameVer() const {
    return arch->getGameVer();
}

QDateTime Report::getDateTime() const {
    return dateTime;
}

QDateTime Report::getSendDate() const {
    return sendDate;
}

bool Report::getFixed() const {
    return fixed;
}

QString Report::getgameVerFix() const {
    return gameVerFix;
}

bool Report::getSent() const {
    return sent;
}

QString Report::getDebugText() const {
    return debugText;
}

QString Report::getUserText() const {
    return userText;
}

QString Report::getTextualDescription() {
    //!
    //!Generates the textual description that will be sent.
    //!

    QString textualReport;

    textualReport.append( "OpenGooDTS Report " + uuid.toString() + " generated on " + dateTime.toString() +"\n"
                          + "-----------------------------------------------------\n"
                          + "-                      Architecture                      -\n"
                          + "-----------------------------------------------------\n"
                          + "- " + arch->getGameVer() + " in " + arch->getBuild() + "\n"
                          + "- " + arch->getOs() + "  on " + arch->getArch() + " with " + arch->getVga() + ", " + arch->getDriverVer() + "\n"
                          + "-----------------------------------------------------\n"
                          + "-                         Report                            -\n"
                          + "-----------------------------------------------------\n"
                          + "-Tracer=\n"
                          + getDebugText()+"\n"
                          + "-UserDescription=\n"
                          + getUserText()+"\n"
                          + "-----------------------------------------------------\n"
                          + "Sent on " + sendDate.toString() + "\n");

    return textualReport;
}

QUuid Report::getUuid() {
    return uuid;
}

Architecture* Report::getArchP() {
    return arch;
}


