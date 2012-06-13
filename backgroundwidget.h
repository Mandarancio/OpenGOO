#ifndef BACKGROUNDWIDGET_H
#define BACKGROUNDWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QList>

#include "background.h"
#include "ground.h"
#include "target.h"

class BackGroundWidget : public QWidget
{
    Q_OBJECT
public:
    explicit BackGroundWidget(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *);
private:
    //Game
    QList <Ground *>ground;         //Ground objects
    QList <BackGround*> background; //background objects
    Target * target; //Target object
    QRadialGradient bgColor;
    QPoint translation;
    QRect limit;
    float scale;
signals:
    
public slots:
    void translated(QPoint p);
    void setTarget(Target* target){
        this->target=target;
    }
    void addGround(Ground* ground);
    void addBackGround(BackGround* bg);
    void setScale(float scale){
        this->scale=scale;
    }
    void setLimit(QRect limit);
    void clear();
};

#endif // BACKGROUNDWIDGET_H
