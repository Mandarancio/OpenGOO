#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QtGui/QMainWindow>
#include "level.h"
#include "levelselector.h"

class MainWidget : public QMainWindow
{
    Q_OBJECT

public:
    MainWidget(QRect geometry,bool debug =false,QWidget *parent = 0);
    ~MainWidget();
private:
    Level *level;
    levelSelector *levelS;

    QRect geometry;

    bool debug;
private slots:
    void levelSelected();
    void backToMainMenu();
};

#endif // MAINWIDGET_H
