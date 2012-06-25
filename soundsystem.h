#ifndef SOUNDSYSTEM_H
#define SOUNDSYSTEM_H

#include <AL/al.h>
#include <AL/alc.h>

#include <QPair>
#include <QPoint>

class SoundSystem
{
public:
    SoundSystem();
    ~SoundSystem();
    bool initialize();
    void setCenter(QPoint p);


    QPair <unsigned int,unsigned int> createSource(ALbyte fileName[]);

    void setVolume(unsigned int source,float volume);
    void setPitch(unsigned int source, float value);
    void setPosition(unsigned int source, QPoint p);


    void playSource(unsigned int source);
    void playWav(ALbyte file[],float volume);


    bool sourceStatus(unsigned int source);

    void deleteSource(QPair<unsigned int ,unsigned int>source);


private:

    QPoint center;

    ALCcontext *context;
    ALCdevice *device;
    bool active;

};

extern SoundSystem soundSystem;


#endif // SOUNDSYSTEM_H
