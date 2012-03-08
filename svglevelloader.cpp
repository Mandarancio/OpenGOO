#include "svglevelloader.h"

#include <QStringList>

#include <QFile>

#include <QDomDocument>
#include <QDomElement>

#include <QDebug>
#include <QRect>
#include <QPolygon>
#include <QPoint>
#include <qmath.h>
#include <QMap>

SvgLevelLoader::SvgLevelLoader(QString path,QSize display,QObject *parent) :
    QObject(parent)
{
    this->path=path;
    displaySize=display;
}


void SvgLevelLoader::setFile(QString path){
    this->path=path;
}

void SvgLevelLoader::setDisplay(QSize size){
    displaySize=size;
}

void SvgLevelLoader::parse(){
    //Setup and open the file
    QFile file(path);
    if (file.open(QFile::ReadOnly)){
        //svg file is an xml file
        //setup the xml document
        QDomDocument doc("svg");
        if (doc.setContent(&file)){
            QDomElement root= doc.documentElement();
            if (root.tagName().compare("svg")!=0){
                qWarning()<<"Parse error, file is not a svg file!";
                emit fileError();
                return;
            }
            int h,w;
            h=root.attribute("height","").toFloat();
            w=root.attribute("width","").toFloat();
            QDomNode node;

            for (int i=0;i<root.childNodes().count();i++){
                node=root.childNodes().at(i);
                if (node.toElement().tagName().compare("title")==0){
                    //parse title and goal
                    QString title;
                    title=node.toElement().text().split('[').at(0);
                    emit levelName(title);
                    //check flag;
                    bool ok=true;
                    QString sGoal=node.toElement().text().split('[').at(1);
                    sGoal=sGoal.remove(']');
                    int goal=sGoal.toInt(&ok);
                    emit levelGoal(goal);
                }
                if (node.toElement().tagName().compare("g")==0){
                    qWarning()<<"Gruppo principale";
                    break;

                }
            }
            QDomElement object;
            for (int i=0;i<node.childNodes().count();i++){
                object=node.childNodes().at(i).toElement();
                qWarning()<<object.tagName();
                QString label,id;
                id=object.attribute("id","");
                label=object.attribute("label","");
                if (!label.length()) label=object.attribute("inkscape:label","");
                if (!label.compare("#startarea")){
                    QString type;
                    int numGOO;
                    bool ok=true;
                    type=id.split('-').at(0);
                    numGOO=id.split('-').at(2).toInt(&ok);
                    if (!ok) {
                        qWarning()<<"Errore";
                        continue;
                    }
                    //compute the type of goo;
                    int nType;
                    if (!type.compare("STD")) nType=0; //STANDARD GOO
                    else if (!type.compare("RMV")) nType=1; //REMOVIBLE GOO
                    else if (!type.compare("FXD")) nType=2; //FIXED GOO
                    QRect rect=parseRect(object);
                    emit levelStartArea(numGOO,rect,nType);
                }
                else if (!label.compare("#limit") || !id.compare("limit")){
                    QRect rect=parseRect(object);
                    //rect.setTopLeft(-rect.topLeft()+QPoint(displaySize.width(),displaySize.height()));
                   // rect.setTopLeft(coordPoint(rect.topLeft()));
                    qWarning()<<"limit"<<rect;
                    emit levelLimit(rect);
                }
                else if (!label.compare("#ground") || !id.compare("ground")){
                    QList <QPoint> pol=parsePointList(object);
                    QPoint center=pol.at(0);
                    pol[0]=QPoint(0,0);
                    emit levelGround(center,pol);

                }
                else if (!label.compare("#target") || !id.compare("target")){
                    QPoint target=parsePoint(object);
                    emit levelTarget(target);
                }
                else if (!label.compare("#joint")){
                    qWarning()<<"Joint"<<id;
                    QPoint a,b;
                    //parse the points
                    if (parsePointList(object).length()>=2){
                        a=parsePointList(object).at(0);
                        b=a+parsePointList(object).at(1);

                    }
                    else emit fileError();

                    qWarning()<<a<<b;

                    QString goos=object.firstChild().toElement().text();
                    qWarning()<<goos;
                }
                else if (!label.compare("#goo")){
                    qWarning()<<object.attribute("id","");
                    //checl flag
                    bool ok=true;
                    //id of the goo
                    int n=object.attribute("id").split("-").at(1).toInt(&ok);
                    //type of the goo
                    QString type=object.attribute("id").split("-").at(0);
                    if (type.length()!=3) ok=false;

                    if (!ok) {
                        qWarning()<<"Goo malformed!";
                        continue;
                    }
                    else {
                        QPoint p=parsePoint(object);
                        qWarning()<<p<<n;
                    }
                }
            }
        }
    }
}

QPoint SvgLevelLoader::parseTransform(QDomElement el){
    QPoint t(0,0);
    //check if the element has a trafnsformation
    if (el.attribute("transform","").length()){
        QString trasf=el.attribute("transform","");
        //check if it has a translation and parse it
        if (trasf.contains("translate")){
            trasf.remove("translate(");
            trasf.remove(')');
            t=strToPoint(trasf);
        }
    }
    return scalePoint(t);
}

QRect SvgLevelLoader::parseRect(QDomElement el){
    QRect r(0,0,0,0);
    //flag for check that the conversion is right
    bool ok=true;
    QPoint p,d;
    //top left point
    p.setX(qRound(el.attribute("x").toFloat(&ok)));
    p.setY(qRound(el.attribute("y").toFloat(&ok)));
    qWarning()<<p;
    p=scalePoint(p);
    qWarning()<<p;

    p=coordPoint(p);
    qWarning()<<p;

    p=p+parseTransform(el);
    qWarning()<<p;

    //size
    d.setX(qRound(el.attribute("width").toFloat(&ok)));
    d.setY(qRound(el.attribute("height").toFloat(&ok)));
    d=scalePoint(d);
    //Check for a translation;
    if (ok) r=QRect(p,p+d);
    return r;
}

QPoint SvgLevelLoader::parsePoint(QDomElement el){
    QPoint p(0,0);
    //flag for check that the conversion is right
    bool ok=true;
    int x,y;
    x=qRound(el.attribute("sodipodi:cx").toFloat(&ok));
    y=qRound(el.attribute("sodipodi:cy").toFloat(&ok));
    if (ok) {
        p=QPoint(x,y);
        p=scalePoint(p);
        p=coordPoint(p)+parseTransform(el);
    }
    return p;
}

QPoint SvgLevelLoader::strToPoint(QString string){
    QPoint p(0,0);
    //flag for check that the conversion is right
    bool ok=true;
    int x,y;
    x=qRound(string.split(',').at(0).toFloat(&ok));
    if (ok) p.setX(x);
    else ok=true;

    y=qRound(string.split(',').at(1).toFloat(&ok));
    if (ok) p.setY(y);

    return p;
}

QList<QPoint> SvgLevelLoader::parsePointList(QDomElement el){
    QList<QPoint> list;
    //compute transformation
    QPoint transform=parseTransform(el);
    //Get the string
    QString str=el.attribute("d","");

    //Check if is a relative path or absolute
    bool relative=true;
    if (str[0]=='M') relative=false;
    //count the number of points
    int nPoint=str.split(' ').count()-1;
    //if is close path the last element is a z
    if (str[str.length()-1]=='z') nPoint--;
    //Start to parse the list;
    QPoint p;
    for (int i=1;i<nPoint+1;i++){
        p=scalePoint(strToPoint(str.split(' ').at(i)));
        if (i==1){
            p=coordPoint(p);
            p+=transform;
        }
        else if (!relative){
            p=coordPoint(p);
            p-=(list.at(0)-transform);
        }
        list.push_back(p);
    }
    return list;
}

//rescale point
QPoint SvgLevelLoader::scalePoint(QPoint p){
    QPoint cP=p;
    //Compute the rescalation factor
    float scaleX=1.0,scaleY=1.0;
    scaleX=float(displaySize.width())/1000.0;
    scaleY=float(displaySize.height())/1000.0;
    //rescale the point
    cP.setX(qRound(scaleX*float(p.x())));
    cP.setY(qRound(scaleY*float(p.y())));
    return cP;
}

//change coordinate of the point
QPoint SvgLevelLoader::coordPoint(QPoint p){
    QPoint cP=p;
    //change the coordinate (from top-left system to a center-down system)
    QPoint center(displaySize.width()/2,displaySize.height()/2);
    qWarning()<<cP<<center<<"coord";
    qWarning()<<cP.x()-center.x();
    cP.setX(cP.x()-center.x());
    cP.setY(center.y()-cP.y());
    return cP;
}
