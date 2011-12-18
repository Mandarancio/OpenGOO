#ifndef MENU_H
#define MENU_H

#include <QObject>
#include <QPainter>
#include <QStringList>

class Menu : public QObject
{
    Q_OBJECT
public:
    explicit Menu(QRect geometry,QObject *parent = 0);
private:
    QStringList index;
    QRect geometry;
    int selected;
    int height;

    void loadMenuFile();
    void computeHeight();
    void paintButton(int ind,QString text,QPainter &p);
signals:

public slots:
    void paint(QPainter &p);
};

#endif // MENU_H
