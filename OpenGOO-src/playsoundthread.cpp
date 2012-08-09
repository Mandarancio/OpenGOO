#include "playsoundthread.h"
#include <QDebug>

PlaySoundThread::PlaySoundThread(QObject *parent) :
    QThread(parent)
{
}

void PlaySoundThread::run() {

    soundSystem=new SoundSystem();
    soundSystem->initialize();
    sSystem=soundSystem;

    char name[100]="./resources/music/opengoo.ogg";
    soundSystem->playOGG(name);

    exec();
}

void PlaySoundThread::stopSong() {
    soundSource = soundSystem->getSource();
    //qWarning() << "Stato Attivo="<< soundSystem->sourceStatus(2);
    soundSystem->stopSource(soundSource);
}

void PlaySoundThread::pauseSong() {
    soundSource = soundSystem->getSource();
    soundSystem->pauseSource(soundSource);
}

void PlaySoundThread::startSong() {
    soundSource = soundSystem->getSource();
    soundSystem->playSource(soundSource);
}

void PlaySoundThread::quit() {

    this->stopSong();
    delete soundSystem;

    QThread::exit(0);
}
