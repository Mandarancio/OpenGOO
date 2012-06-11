#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QCloseEvent>
#include "level.h"
#include "levelselector.h"
#include "backgroundwidget.h"


class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QRect geometry,int flag,QWidget *parent = 0);
    ~MainWidget();
private:
    Level *level;
    LevelSelector *levelS;
    BackGroundWidget * bgWidget;

    QRect geometry;
    int flag;
protected:
    void closeEvent(QCloseEvent *);
private slots:
    void levelSelected();
    void backToMainMenu();
};

#endif // MAINWIDGET_H
