#ifndef QB2DRAW_H
#define QB2DRAW_H
#include <QObject>
#include <QPainter>
#include <Box2D/Box2D.h>

class QB2Draw : public b2Draw, QObject
{

public:
    QB2Draw(QRect displayGeometry,QPainter * p=NULL) :
        painter(p), //For setting the painter
        geometry(displayGeometry), //Set the display geometry (for convert the coordinate system)
        scale(10.0)
    {
    }

    //void setScale(float sx=1.0, float sy=1.0); TODO
    void DrawPolygon(const b2Vec2 *vertices, int32 vertexCount, const b2Color &color); //Draw a polyline
    void DrawSolidPolygon(const b2Vec2 *vertices, int32 vertexCount, const b2Color &color); //Draw a polygone
    void DrawCircle(const b2Vec2 &center, float32 radius, const b2Color &color); //Draw a circonference
    void DrawSolidCircle(const b2Vec2 &center, float32 radius, const b2Vec2 &axis, const b2Color &color); //Draw a circle
    void DrawSegment(const b2Vec2 &p1, const b2Vec2 &p2, const b2Color &color); //Draw a line
    void DrawTransform(const b2Transform &xf); //Set a transformate (translate and rotate)
private:
    QPainter * painter; //The painter
    QRect geometry; //The geometry
    float scale;

    QPoint toQPoint(b2Vec2 vec){ //To convert b2vec2 in QPoint (and also change the coordinate system)
        return QPoint(vec.x*scale,vec.y*scale);
    }
    QColor toQColor(b2Color color){ //To convert b2Color in QColor
        return QColor(color.r,color.g,color.b);
    }

public slots:
    void updateGeometry(QRect displayGeometry){
        geometry=displayGeometry;//For update the geometry (ex:resizing of the windows)
    }
    void setPainter(QPainter * p) {
        painter=p;
    }
};

#endif // QB2DRAW_H
