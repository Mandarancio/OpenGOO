#ifndef SVGLEVELLOADER_H
#define SVGLEVELLOADER_H

#include <QObject>

#include <QDomElement>

#include <QList>
#include <QPoint>
#include <QRect>


#include "goo.h"

class SvgLevelLoader : public QObject
{
    Q_OBJECT
public:
    explicit SvgLevelLoader(QString path="",QSize display=QSize(1000,1000),QObject *parent = 0);
    void setFile(QString path);
    void setDisplay(QSize size);
    //function that parse the svg
    void parse();
private:

    //function to parse a translation from a domelement
    QPoint parseTransform(QDomElement el);
    //function to parse a rect from a domelement
    QRect parseRect(QDomElement el);
    //function to parse a point from a string
    QPoint strToPoint(QString string);
    //function to parse a point from a circle svg
    QPoint parsePoint(QDomElement el);
    //function to parse a list of points from a domelement (the first point is absolute the other are relative at the first)
    QList<QPoint> parsePointList(QDomElement el);

    //point transformation (rescale and change coordinate system)
    QPoint scalePoint(QPoint p);
    QPoint coordPoint(QPoint p);
    //path of the file to open
    QString path;
    //display size
    QSize displaySize;


signals:
    //File not found or not a svg erro signal
    void fileError();

    //LEVEL SETTING SIGNAL
    void levelName(QString name);
    void levelGoal(int goal);
    //GOOS START AREA WITH TYPE
    void levelStartArea(int numberOfBalls,QRect area,int type);
    //void levelJoint(QPoint gooA,QPoint gooB);
    void levelLimit(QRect limit);
    void levelTarget(QPoint target);
    void levelGround(QPoint center,QList<QPoint> groundPoints);
public slots:

};

#endif // SVGLEVELLOADER_H
