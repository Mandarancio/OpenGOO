#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include <QObject>
#include <QRect>
#include <QPoint>
#include <QList>


class LevelLoader : public QObject
{
    Q_OBJECT
public:
    explicit LevelLoader(QString path,QObject *parent = 0);
    void load();
    void setDisplay(int width,int height);
private:
    int wC,hC; //Width and height of the display;
    QString path;
    bool name,goal,limit,ground,target;

    void initFlags();
    bool checkFlags();
    //METHOD TO READ AND PARSE THE FILE
    void readLevel();   //Function to load the level from file
    bool parseString(QString string);   //Split a line of the file in the two component the tag and the real information
    bool parseInfo(QString tag,QString info); //Recognize the tag and use the info
signals:
    //ERROR SIGNAL
    void variableError();
    void fileError();
    void parseError();
    //LEVEL SETTING SIGNAL
    void levelName(QString name);
    void levelGoal(int goal);
    //GOOS START AREA WITH TYPE
    void levelStartArea(int numberOfBalls,QRect area,int type);
    void levelJoint(QPoint gooA,QPoint gooB);
    void levelLimit(QRect limit);
    void levelTarget(QPoint target);
    void levelGround(QPoint center,QList<QPoint> groundPoints);
public slots:

};

#endif // LEVELLOADER_H
