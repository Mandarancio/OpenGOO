#ifndef BACKGROUNDWIDGET_H
#define BACKGROUNDWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QList>

#include "background.h"
#include "ground.h"


class BackGroundWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BackGroundWidget(QWidget *parent = 0);
protected:
    void paintEvent(QPaintEvent *);
private:
    //Game
    QList <Ground *>ground;         //Ground object
    QList <BackGround*> background; //background objects
    QRadialGradient bgColor;
    QPoint translation;
    QRect limit;
    float scale;
signals:
    
public slots:
    void translated(QPoint p);
    void addGround(Ground* ground);
    void addBackGround(BackGround* bg);
    void setScale(float scale);
    void setLimit(QRect limit);
    void clear();
};

#endif // BACKGROUNDWIDGET_H
