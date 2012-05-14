#ifndef MENU_H
#define MENU_H

#include <QObject>

#include <QPainter>
#include <QStringList>
#include <QList>
#include <QMouseEvent>

#include <QGraphicsItem>
#include <QGraphicsScene>

class Menu : public QObject
{
    Q_OBJECT
public:
    explicit Menu(QRect geometry,QObject *parent = 0,bool debug=false);
    void mouseRelease(QMouseEvent *e);
private:
    QStringList index;
    QList<QRect> buttons;
    QGraphicsScene *scene;
    QRect geometry;
    int selected;
    int height;
    bool debug;

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
