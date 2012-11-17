/*
OpenGooDst - Bug reporting system design for OpenGoo game.
Copyright (C) <2012>  <Fabrizio Signoretti - fasigno37@gmail.com>
Under GPLv3.
*/

#include <QApplication>
#include <QMessageBox>
#include <QTranslator>
#include <QWidget>
#include <QDebug>

#include "managergui.h"
#include "wizardmain.h"

void init() {

    //Opens the game configuration file and detects the language set by the user.
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //Qt linguistic code loader:
    // LOAD THE LANGUAGE SET BY THE USER!!!
    QTranslator translator;
//    qWarning() << translator.load("OpenGooDst - src/res/lang/ogDst_it");
//    a.installTranslator(&translator);

    if (argc > 1) {

        if(a.arguments().at(1).compare("-m") == 0) {

            init();
            ManagerGui *m = new ManagerGui;
            m->show();

        } else if (a.arguments().at(1).compare("-w") == 0) {

            if(argc < 3) {
                qWarning() << "Wizard Argument:\n";
                qWarning() << "-uuid=UUID_REPORT";

            } else {

                init();
                QUuid uuid = QUuid(a.arguments().at(2).split("=").last()); //TODO: Check for invalid arguments!!!
                WizardMain *w = new WizardMain(uuid);
                w->show();
            }
        }

    } else {

        qWarning() << "Wrong Arguments!";
        qWarning() << "Use:\n";
        qWarning() << "-w for wizard mode";
        qWarning() << "-m for manager mode\n";
        qWarning() << "Attention, the wizard mode needs further arguments,"
                   << "for more information just launch with -w";

        //TODO: terminate QApplication loop and exit when arguments are wrong!

    }

    return a.exec();
}
