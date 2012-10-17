#ifndef LEVELSELECTOR_H
#define LEVELSELECTOR_H

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QDir>
#include <QKeyEvent>
#include <QPaintEvent>

/*!
 * The LevelSelector class creates a menu that let the user select a level.
 */

class LevelSelector : public QWidget
{
    Q_OBJECT

public:
    explicit LevelSelector(QWidget *parent = 0);
    ~LevelSelector();
    QString getLevelSelected();

protected:
    void mouseReleaseEvent(QMouseEvent *e);
    void keyPressEvent(QKeyEvent *e);
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *);

signals:
    void closing();
    void eventLevelSelected();

private:
    QList<QRect> buttons;
    QRect geometry;
    int buttonHeight;

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
