#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QCloseEvent>
#include "level.h"
#include "levelselector.h"
#include "backgroundwidget.h"
#include "introduction.h"

/*!
 * The MainWidget class is the starting point of the game.
 */

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    MainWidget(QRect geometry,QWidget *parent = 0);
    ~MainWidget();

private:
    Level *level;
    LevelSelector *levelS;
    BackGroundWidget * bgWidget;
    QGridLayout * layout;
    Introduction * intro;

protected:
    void closeEvent(QCloseEvent *);

private slots:
    void levelSelected();
    void backToMainMenu();
    void startSelection();
};

#endif // MAINWIDGET_H
