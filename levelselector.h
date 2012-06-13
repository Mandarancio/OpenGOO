#ifndef LEVELSELECTOR_H
#define LEVELSELECTOR_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QDir>
#include <QKeyEvent>
#include <QPaintEvent>


class LevelSelector : public QWidget
{
    Q_OBJECT
public:
    explicit LevelSelector(QRect geometry,QWidget *parent = 0);
    ~LevelSelector();
    QString getLevelSelected();
protected:
    void mouseReleaseEvent(QMouseEvent *e);
    void keyReleaseEvent(QKeyEvent *e);
    void paintEvent(QPaintEvent *e);
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
    void closeAll(){
        emit this->closing();
    }
};

#endif // LEVELSELECTOR_H
