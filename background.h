#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <QObject>
#include <QList>
#include <QPair>
#include <QPolygon>
#include <QColor>
#include <QPainter>

class BackGround : public QObject
{
    Q_OBJECT
public:
    explicit BackGround(int id=0,QObject *parent = 0);
    //Add a shape!
    void addPolygon(QPolygon polygon, QColor color);
    //set delta
    void setDelta(float delta);
    //get id
    int getID();

private:
    //ID Of the background, usefull for different background.
    int id;
    //Delta is the difference of the translation with the main objects
    float delta;
    //Translate variable
    QPoint translate;
    //List of shape with color
    QRect computeRect();

    QList < QPair<QPolygon, QColor> > polygons;

signals:

public slots:
    //Function to set translate
    void setTranslate(QPoint p);
    //Function to paint
    void paint(QPainter &p);

};

#endif // BACKGROUND_H
