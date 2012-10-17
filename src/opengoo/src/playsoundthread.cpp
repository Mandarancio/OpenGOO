#include <QDebug>

#include "playsoundthread.h"

PlaySoundThread::PlaySoundThread(QObject *parent) :
    QThread(parent)
{
}

void PlaySoundThread::run() {
    //!
    //!The soundSystem object is here initialized and the song is loaded.
    //!

    soundSystem=new SoundSystem();
    soundSystem->initialize();
    sSystem=soundSystem;

    char name[100]="./resources/music/opengoo.ogg";
    soundSystem->playOGG(name);

    exec();
}

void PlaySoundThread::stopSong() {
    //!
    //!Stops the song.
    //!

    soundSource = soundSystem->getSource();
    //qWarning() << "Stato Attivo="<< soundSystem->sourceStatus(2);
    soundSystem->stopSource(soundSource);
}

void PlaySoundThread::pauseSong() {
    //!
    //!Puts the song in pause.
    //!

    soundSource = soundSystem->getSource();
    soundSystem->pauseSource(soundSource);
}

void PlaySoundThread::startSong() {
    //!
    //!Plays the song.
    //!

    soundSource = soundSystem->getSource(); //Get the active source where play the song.
    soundSystem->playSource(soundSource);
}

void PlaySoundThread::quit() {
    //!
    //!Deletes the thread.
    //!

    this->stopSong();
    delete soundSystem;

    QThread::exit(0);
}
