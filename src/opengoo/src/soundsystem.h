#ifndef SOUNDSYSTEM_H
#define SOUNDSYSTEM_H

#include <AL/al.h>
#include <AL/alc.h>

#include <QPair>
#include <QPoint>
#include <QList>
#include <QThread>

#include <vorbis/vorbisfile.h>

/*!
 * The SoundSystem class manages the sound using openAL.
 */
class SoundLoop;

class SoundSystem
{
public:
    SoundSystem();
    ~SoundSystem();

    enum typeSound{scream, boing2, pop, captured, drag};

    bool initialize();
    void setCenter(QPoint p);

    QPair <unsigned int,unsigned int> createPair(const char* fileName);
    QPair <unsigned int,unsigned int> createPair(typeSound type);


    void setVolume(unsigned int source,float volume);
    void setPitch(unsigned int source, float value);
    void setPosition(unsigned int source, QPoint p);

    void playSource(unsigned int source);
    void stopSource(unsigned int source);
    void pauseSource(unsigned int source);

    bool sourceStatus(unsigned int source);

    void addSource(QPair<unsigned int , unsigned int> source);
    void deleteSource(QPair<unsigned int ,unsigned int>source);

    void initMusic(const char* filename, bool isLoopMode=false);
    void delMusic();
    void startMusic();
    void stopMusic();
    void pauseMusic();

private:
    QList<QPair <unsigned int, unsigned int> > sources;
    QPoint center;
    bool active;

    SoundLoop* musicLoop;
    ALuint srcMusic;

    ALuint bufScream;
    ALuint bufBoing2;
    ALuint bufPop;
    ALuint bufCaptured;
    ALuint bufDrag;

    ALuint genBuffer();
    ALuint genSource();
    ALuint createBufferFromFile (const char* filename);
    ALuint createBufferFromOGG (const char* filename);
    bool attachBuffer(ALuint source, ALint buffer);
    void deleteBuffer(ALuint buffer);
    void deleteSource(ALuint source);
};


class SoundLoop : public QThread{
    ALuint sourceID;
    void run()
    {       
        alSourcePlay(sourceID);
    }

public:
    SoundLoop(ALuint source, bool isLoopMode=false)
    {
        sourceID = source;
        if (isLoopMode)
        {
            alSourcei(sourceID, AL_LOOPING, AL_TRUE);
        }
        
    }
};

#endif // SOUNDSYSTEM_H
