#include "soundsystem.h"
#include <AL/alut.h>
#include <QDebug>

SoundSystem::SoundSystem() : center(QPoint(0,0))
{
}

SoundSystem::~SoundSystem(){
    if (!active) return;
    alcDestroyContext(context);
    alcCloseDevice(device);
}

bool SoundSystem::initialize(){

    device = alcOpenDevice(NULL);
    if (device == NULL)
    {
        qWarning("OpenAL problem! No audio device found!");
        return false;
    }
    active=true;
    //Create a context
    context=alcCreateContext(device,NULL);

    //Set active context
    alcMakeContextCurrent(context);

    return true;
}

void SoundSystem::setCenter(QPoint p){
    center=p;
}

QPair<unsigned int,unsigned int> SoundSystem::createSource(ALbyte fileName[]){

    QList <int> toRemove;
    for (int i=0;i<sources.length();i++) {
        if (!sourceStatus(sources[i].first)) {
            deleteSource(sources[i]);
            toRemove.push_back(i);
        }
    }
    for (int i=0;i<toRemove.length();i++)
        sources.removeAt(toRemove[i]);

    if (!active) return QPair<unsigned int, unsigned int>(0,0);
    char*     alBuffer;             //data for the buffer
    ALenum alFormatBuffer;    //buffer format
    ALsizei   alFreqBuffer;       //frequency
    long       alBufferLen;        //bit depth
    ALboolean    alLoop;         //loop
    unsigned int alSource;      //source
    unsigned int alSampleSet;

    alutLoadWAVFile(fileName,&alFormatBuffer, (void **) &alBuffer,(ALsizei *)&alBufferLen, &alFreqBuffer, &alLoop);
    alGenSources(1, &alSource);

    //create  buffer
    alGenBuffers(1, &alSampleSet);

    //put the data into our sampleset buffer
    alBufferData(alSampleSet, alFormatBuffer, alBuffer, alBufferLen, alFreqBuffer);

    //assign the buffer to this source
    alSourcei(alSource, AL_BUFFER, alSampleSet);
    alutUnloadWAV(alFormatBuffer,alBuffer,alBufferLen,alFreqBuffer);

    return QPair<unsigned int, unsigned int>(alSource,alSampleSet);
}

void SoundSystem::setVolume(unsigned int source, float volume){
    if (!active) return;
    alSourcef(source,AL_GAIN,volume);
}

void SoundSystem::setPitch(unsigned int source, float value){
    if (!active) return;
    alSourcef(source,AL_PITCH,value);
}

void SoundSystem::playSource(unsigned int source){
    if (!active) return;
    alSourcePlay(source);
}

void SoundSystem::stopSource(unsigned int source){
    if (!active) return;
    alSourceStop(source);
}

void SoundSystem::playWav(ALbyte file[], float volume){
    if (!active) return;
    char*     alBuffer;             //data for the buffer
    ALenum alFormatBuffer;    //buffer format
    ALsizei   alFreqBuffer;       //frequency
    long       alBufferLen;        //bit depth
    ALboolean    alLoop;         //loop
    unsigned int alSource;      //source
    unsigned int alSampleSet;

    alutLoadWAVFile(file,&alFormatBuffer, (void **) &alBuffer,(ALsizei *)&alBufferLen, &alFreqBuffer, &alLoop);
    alGenSources(1, &alSource);

    //create  buffer
    alGenBuffers(1, &alSampleSet);

    //put the data into our sampleset buffer
    alBufferData(alSampleSet, alFormatBuffer, alBuffer, alBufferLen, alFreqBuffer);

    //assign the buffer to this source
    alSourcei(alSource, AL_BUFFER, alSampleSet);

    alSourcef(alSource,AL_GAIN,volume);
    alSourcePlay(alSource);
    alutUnloadWAV(alFormatBuffer,alBuffer,alBufferLen,alFreqBuffer);
   // alDeleteSources(1,&alSource);
    alDeleteBuffers(1,&alSampleSet);

}

bool SoundSystem::sourceStatus(unsigned int source){
    if (!active) return false;
    ALenum state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);
    return (state == AL_PLAYING);
}

void SoundSystem::addSource(QPair<unsigned int, unsigned int> source){
    if (!sources.contains(source))
        sources.push_back(source);
}

void SoundSystem::deleteSource(QPair<unsigned int,unsigned int> source){
    if (!active) return;
    alDeleteSources(1,&source.first);
    alDeleteBuffers(1,&source.second);
}

void SoundSystem::setPosition(unsigned int source, QPoint p){
    if (!active) return;

    alSource3f(source,AL_POSITION,p.x()-center.x(),p.y()-center.y(),0);
    //alSourcei(source,AL_SOURCE_RELATIVE,AL_TRUE);
}
