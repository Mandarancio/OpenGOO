#ifndef MENU_H
#define MENU_H

#include <QObject>

#include <QPainter>
#include <QStringList>
#include <QList>
#include <QMouseEvent>

class Menu : public QObject
{
    Q_OBJECT
public:
    explicit Menu(QRect geometry,QObject *parent = 0);
    void mouseRelease(QMouseEvent *e);
private:
    QStringList index;
    QList<QRect> buttons;
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
public slots:
    void paint(QPainter &p);
};

#endif // MENU_H
