#ifndef LEVELSELECTOR_H
#define LEVELSELECTOR_H

#include <QGLWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QDir>

class levelSelector : public QGLWidget
{
    Q_OBJECT
public:
    explicit levelSelector(QRect geometry,QWidget *parent = 0);
    ~levelSelector();
    QString getLevelSelected();
protected:
    void mouseReleaseEvent(QMouseEvent *e);
signals:
    void closing();
    void eventLevelSelected();
private:


    QList<QRect> buttons;
    QRect geometry;
    int height;

    QStringList levels;
    int selected;



    void findLevels();
    void paintButton(int ind, QPainter &p );
    void computeHeight();
    void paint(QPainter &p);

private slots:
    void closeAll();
};

#endif // LEVELSELECTOR_H
