    #include "tools.h"
#include <QDebug>

b2Vec2 toVec(QPoint p){
    return b2Vec2(p.x(),p.y());
}

QPoint toPoint(b2Vec2 v){
    return QPoint(v.x,v.y);
}

b2Vec2* toVecs(QList<QPoint> ps){
    b2Vec2 vecs[ps.length()];
    for (int i=0;i<ps.length();i++){
        vecs[i]=toVec(ps[i]);
    }
    return vecs;
}

QPolygon toPoly(QList<QPoint> ps){
    QPolygon poly(ps.length());
    for (int i=0;i<ps.length();i++){
        poly.insert(i,ps[i]);
    }
    return poly;
}

QPolygon toPoly(QList<QPoint> ps,QPoint center){
    QPolygon poly;
    for (int i=0;i<ps.length();i++){
        poly.insert(i,center+ps[i]);
    }
    return poly;
}



QPoint pPoint(QString s, bool &ok,int w,int h){
    QPoint p;
    if (s.split(' ').length()!=2) {
        ok=false;
        return p;
    }
    p.setX(qRound(s.split(' ').at(0).toFloat(&ok)*w/1000));
    if (!ok) return p;
    p.setY(qRound(s.split(' ').at(1).toFloat(&ok)*h/1000));
    return p;
}

QSize pSize(QString s, bool &ok){
    QSize sz;
    if (s.split(' ').length()!=2) {
        ok=false;
        return sz;
    }
    sz.setWidth(s.split(' ').at(0).toInt(&ok));
    if (!ok) return sz;
    sz.setHeight(s.split(' ').at(1).toInt(&ok));
    return sz;
}

QRect pRect(QString s, bool &ok,int w,int h){
    QRect r;
    if (s.split(',').length()!=2){
        ok=false;
        return r;
    }
    r.setTopLeft(pPoint(s.split(',').at(0),ok,w,h));
    if (!ok) return r;
    r.setSize(pSize(s.split(',').at(1),ok));
    return r;
}

QList<QPoint> pList(QString s, bool &ok,int w,int h, int min){
    QList <QPoint> ls;
    int l=s.split(',').length();
    if (l<min){
        ok=false;
        return ls;
    }
    for (int i=0;i<l;i++){
        ls.push_back(pPoint(s.split(',').at(i),ok,w,h));
        if (!ok) return ls;
    }
    return ls;
}



