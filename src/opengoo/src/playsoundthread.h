#ifndef PLAYSOUNDTHREAD_H
#define PLAYSOUNDTHREAD_H

#include <QThread>

#include "soundsystem.h"
#include "publicclass.h"

class PlaySoundThread : public QThread
{
    Q_OBJECT

    SoundSystem *soundSystem;
    ALuint soundSource;

public:
    explicit PlaySoundThread(QObject *parent = 0);

     void run();
     void quit();
     void stopSong();
     void pauseSong();
     void startSong();
    
signals:
    
public slots:
    
};

#endif // PLAYSOUNDTHREAD_H
