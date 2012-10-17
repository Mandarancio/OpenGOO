#ifndef SVGLEVELLOADER_H
#define SVGLEVELLOADER_H

#include <QDomElement>
#include <QObject>
#include <QList>
#include <QPoint>
#include <QRect>
#include <QPair>

#include "goo.h"

class SvgLevelLoader : public QObject
{
    Q_OBJECT

public:
    explicit SvgLevelLoader(QString path="", QObject *parent = 0) :
        QObject(parent),
        path(path)
    {
    }

    void setFile(QString path){
        this->path=path;
    }

    //function that parse the svg
    bool parse();
    //function to add a created goo
    void addGoo(int id, Goo* goo);

private:
    int h,w;
    QPoint translation;

    //function that return the list index of a goo from id (-1 not found)
    int getIndex(int id);

    //Parse background object
    void parseBackground(QDomElement el);
    //Parse ground object
    //void parseGround(QDomElement el);

    //function to parse a translation from a domelement
    QPoint parseTransform(QDomElement el);
    //function to parse a rect from a domelement
    QRect parseRect(QDomElement el,bool cal=false);
    //function to parse a point from a string
    QPoint strToPoint(QString string);
    //function to parse a point from a circle svg
    QPoint parsePoint(QDomElement el);
    //function to parse a list of points from a domelement (the first point is absolute the other are relative at the first)
    QList<QPoint> parsePointList(QDomElement el);
    //Function to parse the fill color
    QColor parseFill(QDomElement el);


    // point rescalation
    // QPoint scalePoint(QPoint p);

    //path of the file to open
    QString path;

    //List to store links to be created
    QList<QPair<int,int> > links;
    //List to store created goo
    QList<QPair<int,Goo*> > goos;


signals:
    //File not found or not a svg erro signal
    void fileError();

    //LEVEL SETTING SIGNAL
    void levelName(QString name);
    void levelGoal(int goal);
    //GOOS START AREA WITH TYPE
    void levelStartArea(int numberOfBalls,QRect area,int type);
    void levelLimit(QRect limit);
    void levelTarget(QPoint target);
    void levelGround(QPoint center,QList<QPoint> groundPoints);
    void levelGOO(QPoint center,int id,int type);
    void levelJoint(Goo* a,Goo *b);
    void levelGeometry(QSize size);
    //add a thorn
    void addLevelThorn(QPoint center,QList<QPoint> thornPoints);
    //add a shape to a bg object
    void addBackGroundShape(int id,QPolygon poly,QColor color);

public slots:

};

#endif // SVGLEVELLOADER_H
