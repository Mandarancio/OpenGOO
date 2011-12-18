#ifndef MENU_H
#define MENU_H

#include <QObject>
#include <QPainter>

class Menu : public QObject
{
    Q_OBJECT
public:
    explicit Menu(QRect geometry,QObject *parent = 0);
private:
    QRect geometry;
signals:

public slots:
    void paint(QPainter &p);
};

#endif // MENU_H
