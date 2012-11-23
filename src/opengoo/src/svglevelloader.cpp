#include "svglevelloader.h"

#include <QStringList>

#include <QFile>

#include <QDomDocument>
#include <QDomElement>

#include <QDebug>
#include <logger.h>
#include <QSize>
#include <QRect>
#include <QPolygon>
#include <QPoint>
#include <qmath.h>
#include <QMap>
#include <QRgb>

#include "tools.h"

bool SvgLevelLoader::parse(){
    links.clear();
    goos.clear();
    //Setup and open the file
    QFile file(path);
    if (file.open(QFile::ReadOnly)){
        //svg file is an xml file
        //setup the xml document
        QDomDocument doc("svg");
        if (doc.setContent(&file)){
            QDomElement root= doc.documentElement();
            if (root.tagName().compare("svg")!=0){
                logWarn(QString("Parse error, file %1 is not a svg file!").arg(path));
                emit fileError();
                return false;
            }
            h=root.attribute("height","").toFloat();
            w=root.attribute("width","").toFloat();
            emit levelGeometry(QSize(w,h));
            QDomNode node;

            for (int i=0;i<root.childNodes().count();i++){
                node=root.childNodes().at(i);
                if (node.toElement().tagName().compare("title")==0){
                    //parse title and goal
                    QString title;
                    title=node.toElement().text().split('[').at(0);
                    emit levelName(title);
                    //check flag;
//                    bool ok=true;
//                    QString sGoal=node.toElement().text().split('[').at(1);
//                    sGoal=sGoal.remove(']');
//                    int goal=sGoal.toInt(&ok);
//                    emit levelGoal(goal);
                }
                if (node.toElement().tagName().compare("g")==0){
                 //   logWarn()<<"Gruppo principale";
                    translation=this->parseTransform(node.toElement());
                    break;

                }
            }
            QDomElement object;
            for (int i=0;i<node.childNodes().count();i++){
                object=node.childNodes().at(i).toElement();
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
                        logWarn("Errore");
                        continue;
                    }
                    //compute the type of goo;
                    int nType=0;
                    if (!type.compare("STD")) nType=0; //STANDARD GOO
                    else if (!type.compare("RMV")) nType=1; //REMOVIBLE GOO
                    else if (!type.compare("FXD")) nType=2; //FIXED GOO
                    else if (!type.compare("BLN")) nType=3; //BALLOON GOO
                    else if (!type.compare("STK")) nType=4; //STICKY GOO
                    QRect rect=parseRect(object);
                    emit levelStartArea(numGOO,rect,nType);
                }
                else if (!label.compare("#limit") || !id.compare("limit")){
                    QRect rect=parseRect(object);
                    rect.setY(rect.y()-h);
                    emit levelLimit(rect);
                }
                else if (!label.compare("#ground") || !id.compare("ground")){
                    QList <QPoint> pol=parsePointList(object);
                    QPoint center=pol[0];
                    pol[0]=QPoint(0,0);
                    emit levelGround(center,pol);

                }
                else if (!label.compare("#thorn") || !id.compare("thorn")){
                    QList <QPoint> pol=parsePointList(object);
                    QPoint center=pol[0];
                    pol[0]=QPoint(0,0);
                    emit addLevelThorn(center,pol);
                }
                else if (!label.compare("#target") && id.split(':').length()==2 ){
                    QPoint target=parsePoint(object);
                    bool ok=true;
                    int goal=id.split(':').at(1).toInt(&ok);
                    if (ok) emit levelGoal(goal);
                    emit levelTarget(target);
                }
                else if (!label.compare("#joint")){

                    QString goos=object.firstChild().toElement().text();
                    int a,b;
                    //check flag
                    bool ok=true;
                    //parse goo id to connect
                    if (goos.split('-').length()!=2) {
                        continue;
                    }
                    a=goos.split('-').at(0).toInt(&ok);
                    b=goos.split('-').at(1).toInt(&ok);
                    if (ok){
                        QPair <int,int> link;
                        link.first=a;
                        link.second=b;
                        links.push_back(link);

                    }
                }
                else if (!label.compare("#goo")){
                    //checl flag
                    bool ok=true;
                    //id of the goo
                    int n=object.attribute("id").split("-").at(1).toInt(&ok);
                    //type of the goo
                    QString type=object.attribute("id").split("-").at(0);
                    if (type.length()!=3) ok=false;

                    if (!ok) {
                        logWarn("Goo malformed!");
                        continue;
                    }
                    else {
                        QPoint p=parsePoint(object);
                        int nType=0;
                        if (!type.compare("STD")) nType=0; //STANDARD GOO
                        else if (!type.compare("RMV")) nType=1; //REMOVIBLE GOO
                        else if (!type.compare("FXD")) nType=2; //FIXED GOO
                        else if (!type.compare("BLN")) nType=3; //BALLOON GOO
                        else if (!type.compare("STK")) nType=4; //STICKY GOO

                        emit levelGOO(p,n,nType);
                    }
                }
                else if(!label.split('-').at(0).compare("#background")){
                    parseBackground(object);
                }
            }
            //emit link / joint signal
            int a,b;

            for (int i=0;i<links.length();i++){

                a=getIndex(links[i].first);
                b=getIndex(links[i].second);
                if (a>=0 && b>=0) emit levelJoint(goos[a].second,goos[b].second);
            }
            return true;
        }
        else {
            logWarn(QString("File %1 is not an xml! Error!").arg(path));
            emit fileError();
            return false;
        }
    }
    else {
        logWarn(QString("File %1 not found! Error!").arg(path));
        emit fileError();
        return false;
    }
}

//Function to add a created goo
void SvgLevelLoader::addGoo(int id, Goo *goo){
    QPair <int , Goo*> pair;
    pair.first=id;
    pair.second=goo;
    goos.push_back(pair);
}

void SvgLevelLoader::parseBackground(QDomElement el){
    bool ok=true;
    int id=el.attribute("inkscape:label","").split('-').at(1).toInt(&ok);
    if (!ok) return;
    QList <QPoint> poly=parsePointList(el);
    if (poly.length()){
        QPoint center=poly[0];
        poly[0]=QPoint(0,0);
        QPolygon polygon=toPoly(poly,center);
        QColor fill= parseFill(el);
        emit addBackGroundShape(id,polygon,fill);
    }
    else return;
}

int SvgLevelLoader::getIndex(int id){
    for (int i=0;i<goos.length();i++){
        if (goos[i].first==id) return i;
    }
    return -1;
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
    return t;
}

QRect SvgLevelLoader::parseRect(QDomElement el,bool cal){
    QRect r(0,0,0,0);
    //flag for check that the conversion is right
    bool ok=true;
    QPoint p,d;
    //top left point
    p.setX(qRound(el.attribute("x").toFloat(&ok)));
    if (!cal) p.setY(qRound(el.attribute("y").toFloat(&ok)));
    else p.setY(h-qRound(el.attribute("y").toFloat(&ok)));
    p=p+parseTransform(el)+translation;
    //size
    d.setX(qRound(el.attribute("width").toFloat(&ok)));
    d.setY(qRound(el.attribute("height").toFloat(&ok)));
    d=d;
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
        p=QPoint(x,y)+parseTransform(el)+translation;
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
    QPoint transform=parseTransform(el)+translation;
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
    int skip=0;
    for (int i=1;i<nPoint+1;i++){
        if (str.split(' ').at(i)[0]=='l') {
            relative=true;
            skip++;
            continue;
        }
        else if (str.split(' ').at(i)[0]=='L'){
            relative=false;
            skip++;
            continue;
        }
        if (!((str.split(' ').at(i)[0]>='0' &&  str.split(' ').at(i)[0]<='9')|| str.split(' ').at(i)[0]=='-' )) {
            if (i>2) list.push_back(list[i-2-skip]);
            logWarn(str.split(' ').at(i));
            continue;
        }
        else {
            p=strToPoint(str.split(' ').at(i));
            if (i==1){
                p+=transform;
            }
            else if (!relative){
                p+=transform;
                p-=(list.at(0));
            }
            else if (relative && list.count()) {
                p+=(i>2? list.at(i-2-skip):QPoint(0,0));
            }
            list.push_back(p);
        }
    }
    //) logWarn()<<list;
    return list;
}

QColor SvgLevelLoader::parseFill(QDomElement el){
    QString style=el.attribute("style");
    QString tag;
    QColor color(0,0,0);
    for (int i=0;i<style.split(';').count();i++){
        tag=style.split(';').at(i).split(':').at(0);
        if (tag.compare("fill")==0){
            QString value=style.split(';').at(i).split(':').at(1);

            value.remove('#');
            //logWarn()<<value;
            int r,g,b;
            bool ok=true;
            r=value.mid(0,2).toInt(&ok,16);
            g=value.mid(2,2).toInt(&ok,16);
            b=value.mid(4,2).toInt(&ok,16);

            //logWarn()<<r<<g<<b;
            color=QColor(r,g,b);
        }
    }
    return color;
}
