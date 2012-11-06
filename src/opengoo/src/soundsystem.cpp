#include "soundsystem.h"
#include <AL/alut.h>
#include <QFileInfo>
#include <QDebug>
#include <vector>

SoundSystem::SoundSystem()
{
    active=false;
    bufScream = bufBoing2 = bufPop = bufCaptured = bufDrag = NULL;
}

SoundSystem::~SoundSystem()
{
    if (active) alutExit();
}

bool SoundSystem::initialize()
{
    active = alutInit(NULL, NULL);
    return active;
}

void SoundSystem::setCenter(QPoint p)
{
    center=p;
}

QPair<unsigned int,unsigned int> SoundSystem::createPair(const char* fileName)
{
    if (!active) return QPair<unsigned int, unsigned int>(0,0);
    ALuint source;          //source
    ALuint buffer;       // buffer

    // [1] create  buffer and source
    buffer = createBufferFromFile(fileName);
    source = genSource();

    // [2] attach the buffer to source
    attachBuffer(source, buffer);

    return QPair<unsigned int, unsigned int>(source, buffer);
}

QPair<unsigned int,unsigned int> SoundSystem::createPair(typeSound type)
{
    if (!active) return QPair<unsigned int, unsigned int>(0,0);
    ALuint buffer;          // Buffer ID
    ALuint source;          // Source ID

    // [1] Create  buffer and source
    source = genSource();

    // [2] Attach the buffer to source
    switch(type)
    {
        case scream:
         if(bufScream == NULL) bufScream = createBufferFromFile("resources/sounds/scream.wav");
         buffer = bufScream;
         break;
        case boing2:
            if(bufBoing2 == NULL) bufBoing2 = createBufferFromFile("resources/sounds/boing2.wav");
            buffer = bufBoing2;
            break;
        case pop:
            if(bufPop == NULL) bufPop = createBufferFromFile("resources/sounds/pop.wav");
            buffer = bufPop;
            break;
        case captured:
            if(bufCaptured == NULL) bufCaptured = createBufferFromFile("resources/sounds/captured.wav");
            buffer = bufCaptured;
            break;
        case drag:
            if(bufDrag == NULL) bufDrag = createBufferFromFile("resources/sounds/drag.wav");
            buffer = bufDrag;
            break;
        default:
            buffer = NULL;
            break;
    }

    attachBuffer(source, buffer);

    return QPair<unsigned int, unsigned int>(source, buffer);
}

void SoundSystem::setVolume(unsigned int source, float volume)
{
    if (active) alSourcef(source,AL_GAIN,volume);
}

void SoundSystem::setPitch(unsigned int source, float value)
{
    if (active) alSourcef(source,AL_PITCH,value);
}

void SoundSystem::playSource(unsigned int source)
{
    if (active) alSourcePlay(source);
}

void SoundSystem::stopSource(unsigned int source)
{
    if (active) alSourceStop(source);
}

void SoundSystem::pauseSource(unsigned int source)
{
    if(active) alSourcePause(source);
}

bool SoundSystem::sourceStatus(unsigned int source)
{
    if (!active) return false;

    ALenum state;

    alGetSourcei(source, AL_SOURCE_STATE, &state);

    return (state == AL_PLAYING);
}

void SoundSystem::addSource(QPair<unsigned int, unsigned int> source)
{
    if (!sources.contains(source)) sources.push_back(source);
}

void SoundSystem::deleteSource(QPair<unsigned int,unsigned int> source)
{
    if (active) {
        deleteSource(source.first);
        deleteBuffer(source.second);
    }
}

void SoundSystem::setPosition(unsigned int source, QPoint p)
{
    if (active) alSource3f(source,AL_POSITION,p.x()-center.x(),p.y()-center.y(),0);
}

ALuint SoundSystem::genBuffer()
{
    alGetError(); // clear error code

    ALuint buffer;

    alGenBuffers(1, &buffer);

    if (alGetError() != AL_NO_ERROR) return -1;

    return buffer;
}

ALuint SoundSystem::genSource()
{
    ALuint source;

    alGenSources(1, &source);
    if (alGetError() != AL_NO_ERROR) return -1;

    return source;
}

bool SoundSystem::attachBuffer(ALuint source, ALint buffer)
{
    alSourcei(source, AL_BUFFER, buffer);
    if (alGetError() != AL_NO_ERROR) return false;

    return true;
}

ALuint SoundSystem::createBufferFromOGG (const char *filename)
{
    OggVorbis_File vorbisFile;

    if(ov_fopen(filename, &vorbisFile) < 0) return -1;

    vorbis_info *pInfo = ov_info(&vorbisFile, -1);

    ALenum format;              // The sound data format

    // Check the number of channels... always use 16-bit samples
    if (pInfo->channels == 1)
        format = AL_FORMAT_MONO16;
    else
        format = AL_FORMAT_STEREO16;

    long bytes;
    const int BUFFER_SIZE = 4096;
    char vorbisfileBuffer[BUFFER_SIZE];    // Local fixed size array
    int bitStream;
    std::vector < char > bufferData; // The sound buffer data from file

    do {
        // Read up to a buffer's worth of decoded sound data
        bytes = ov_read(&vorbisFile, vorbisfileBuffer, BUFFER_SIZE, 0, 2, 1, &bitStream);
        // Append to end of buffer
        bufferData.insert(bufferData.end(), vorbisfileBuffer, vorbisfileBuffer + bytes);
    } while (bytes > 0);

    ALuint bufferID = genBuffer();            // The OpenAL sound buffer ID
    alBufferData(bufferID, format, &bufferData[0],  (ALsizei)bufferData.size(), pInfo->rate);
    ov_clear(&vorbisFile);

    return bufferID;
}

ALuint SoundSystem::createBufferFromFile (const char *filename)
{
    ALuint buffer;
    QFileInfo fi(filename);
    QString ext = fi.suffix();

    if(ext.compare("ogg", Qt::CaseInsensitive) == 0) buffer = createBufferFromOGG (filename);
    else if (ext.compare("wav", Qt::CaseInsensitive) == 0) buffer = alutCreateBufferFromFile(filename);
    else return -1;

    return buffer;
}

void SoundSystem::deleteBuffer(ALuint buffer)
{
    alDeleteBuffers(1, &buffer);
}

void SoundSystem::deleteSource(ALuint source)
{
    alDeleteSources(1, &source);
}
void SoundSystem::initMusic(const char* filename, bool isLoopMode)
{
    ALuint buffer; // The OpenAL sound buffer ID

    // [1] Create sound buffer and source
    buffer = createBufferFromFile(filename);
    srcMusic = genSource();

    // [2] Attach sound buffer to source
    attachBuffer(srcMusic, buffer);

    musicLoop = new SoundLoop(srcMusic, isLoopMode);
}

void SoundSystem::delMusic()
{
    stopMusic();
    delete musicLoop;
}

void SoundSystem::startMusic()
{
    musicLoop->start();
}

void SoundSystem::stopMusic()
{
    alSourceStop(srcMusic);
}

void SoundSystem::pauseMusic()
{
    alSourcePause(srcMusic);
}
