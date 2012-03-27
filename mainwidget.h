#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QGLWidget>
#include "level.h"
#include "levelselector.h"
#include <QCloseEvent>

class MainWidget : public QGLWidget
{
    Q_OBJECT

public:
    MainWidget(QRect geometry,bool debug =false,bool multiwindow=false,QWidget *parent = 0);
    ~MainWidget();
private:
    Level *level;
    LevelSelector *levelS;

    QRect geometry;

    bool debug;
    bool multiwindow;
protected:
    void closeEvent(QCloseEvent *);
private slots:
    void levelSelected();
    void backToMainMenu();

};

#endif // MAINWIDGET_H
