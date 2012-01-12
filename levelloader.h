#ifndef LEVELLOADER_H
#define LEVELLOADER_H

#include <QObject>

class LevelLoader : public QObject
{
    Q_OBJECT
public:
    explicit LevelLoader(QString path,QObject *parent = 0);

signals:

public slots:

};

#endif // LEVELLOADER_H
