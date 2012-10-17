#ifndef MENU_H
#define MENU_H

#include <QObject>

#include <QPainter>
#include <QStringList>
#include <QList>
#include <QMouseEvent>

#include <QGraphicsItem>
#include <QGraphicsScene>

/*!
 * The class 'menu' draws the game menu reading the entries in menu.index file located in 'resources' folder.
 */

class Menu : public QObject
{
    Q_OBJECT
public:
    explicit Menu(QRect geometry,QObject *parent = 0);
    void mouseRelease(QMouseEvent *e);
    void setGeometry(QRect geometry);
private:
    QStringList index;
    QList<QRect> buttons;
    QGraphicsScene *scene;
    QRect geometry;
    int selected;
    int height;

    void loadMenuFile();
    void computeHeight();
    void paintButton(int ind,QPainter &p);
signals:
    void eventClose();
    void eventResume();
    void eventRestart();
    void eventBackToMainMenu();
public slots:
    void paint(QPainter &p);
};

#endif // MENU_H
