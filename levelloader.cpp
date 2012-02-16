#include "levelloader.h"

#include <QTextStream>
#include <QFile>
#include <tools.h>
#include <QDebug>

LevelLoader::LevelLoader(QString path,QObject *parent) :
    QObject(parent)
{
    initFlags();
    this->path=path;//Path of the level;
    wC=0;
    hC=0;
}

void LevelLoader::load(){ //trigger to start to load the level!
    initFlags();
    readLevel();
    if (!checkFlags()) emit fileError();
}

void LevelLoader::setDisplay(int width, int height){
    wC=width;
    hC=height;
}

void LevelLoader::initFlags(){
    name=false;
    target=false;
    goal=false;
    ground=false;
    limit=false;
}

bool LevelLoader::checkFlags(){
    if (limit&&name&&ground&&goal&&target) return true;
    else return false;
}

void LevelLoader::readLevel(){
    if (!wC || !hC) {
        qWarning()<<"Display variable has to be initialized";
        emit variableError();
        return;
    }
    QFile file(path);
    if (file.open(QFile::ReadOnly)){
        QTextStream input(&file);
        QString tmp;
        while (!input.atEnd()){
            tmp=input.readLine();//Read line by line
            if (tmp[0]=='#') continue; //COMMENT SKIPPING
            if (!parseString(tmp)) qWarning()<<"Parsed filed on line:"<<tmp; //Parse the string!

        }
        file.close();
    }
    else { // If level file not found
        qWarning()<<"File not found!!";
        emit this->fileError(); //Signal for dangerous file errors
    }
}

bool LevelLoader::parseString(QString string)
{
    QString tag=string.split(':').at(0); //The first part of the string is the tag (the type of information)
    QString info; //The second one is the real information
    if (string.split(':').length()==2) info=string.split(':').at(1); // *ALL* level configuration are splited in two with the ':' char
    else{
        qWarning()<<"File malformated";
        emit this->parseError(); //The file are not conform with us parse policy
        return false;
    }
    if (parseInfo(tag,info)) return true; //Parse the information
    else return false;
}

bool LevelLoader::parseInfo(QString tag, QString info){
    if (!tag.compare("name",Qt::CaseInsensitive) && !name){ //LEVEL NAME
        emit levelName(info);
        name=true;
        return true;
    }
    if (!tag.compare("goal",Qt::CaseInsensitive) && !goal){ //GOAL [MIN. NUMBER OF GOOS TO COLLECT]
        bool ok;
        int goal=info.toInt(&ok);
        if (ok) {
            this->goal=true;
            emit levelGoal(goal);
        }
        return ok;
    }
    if (!tag.compare("start area",Qt::CaseInsensitive)){ //GOOS START AREA (it comprend a rectangular area and the number of goos inside
        if (info.split('|').length()>=2){
            //Only number of balls and area are definied
            if (info.split('|').length()==2){
                QString n,rect;
                n=info.split('|').at(0); //Number
                rect=info.split('|').at(1); //Area
                if (rect.split(',').length()!=2) return false;
                bool ok=true;
                int nb=n.toInt(&ok); //NUMBER OF BALLS
                QRect startArea=pRect(rect,ok,wC,hC); //RECT AREA
                if (ok) emit    levelStartArea(nb,startArea,0);
                return ok;
            }
            //In this case also the type of goo are definied
            else if (info.split('|').length()==3){
                QString n,rect,type;
                n=info.split('|').at(0); //Number of goos
                rect=info.split('|').at(1); //Area where they will be created
                type=info.split('|').at(2); //Type of goo
                int nType=-1; //the codified type;
                //Start to compare the choose type
                if (!type.compare("STD")) nType=0; //STANDARD GOO
                else if (!type.compare("RMV")) nType=1; //REMOVIBLE GOO
                else if (!type.compare("FXD")) nType=2; //FIXED GOO

                if (rect.split(',').length()!=2) return false;
                bool ok=true;
                int nb=n.toInt(&ok); //NUMBER OF BALLS
                QRect startArea=pRect(rect,ok,wC,hC); //RECT AREA
                if (ok) emit   levelStartArea(nb,startArea,nType);
                return ok;
            }
            else return false;
        }
        else return false;
    }
    if (!tag.compare("limit",Qt::CaseInsensitive) && !limit){//Limit of the level
        if (info.split(',').length()==2){
            bool ok=true;
            QRect limit=QRect(pList(info,ok,wC,hC,2).at(0),pList(info,ok,wC,hC,2).at(1));
            if (ok){
                this->limit=true;
                emit levelLimit(limit);
            }
            return ok;
        }
        else return false;
    }
    if (!tag.compare("start joint",Qt::CaseInsensitive)){ //FIRST JOINT OF THE LEVEL
        if (info.split(',').length()==2){
            bool ok=true;
            QPoint a,b;
            a=pList(info,ok,wC,hC,2).at(0);
            b=pList(info,ok,wC,hC,2).at(1);
            if (ok) emit levelJoint(a,b);
            return ok;
        }
        else return false;
    }
    if (!tag.compare("target",Qt::CaseInsensitive)&&!target){ //WHERE IS The target
        bool ok;
        QPoint tPoint=pPoint(info,ok,wC,hC);
        if (ok) {
            this->target=true;
            emit levelTarget(tPoint);
        }
        return ok;
    }
    if (!tag.compare("ground",Qt::CaseInsensitive)&&!ground){ //THE DEFINITION OF THE GROUND!
        if (info.split('|').length()!=2) return false;
        QString center,list;
        center=info.split('|').at(0);
        list=info.split('|').at(1);
        bool ok=true;
        QPoint gCenter=pPoint(center,ok,wC,hC); //CENTER OF THE GROUND

        if (!ok) return false;
        QList <QPoint> gList=pList(list,ok,wC,hC,2); //LIST OF POINT TO DEFINE THE GROUND
        if (ok){
            this->ground=true;
            emit levelGround(gCenter,gList);
        }
        return ok;
    }
    return false;
}
