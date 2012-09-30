#include "soundsystem.h"
#include <AL/alut.h>
#include <QDebug>
#include <vector>
#define BUFFER_SIZE  512 //32768     // 32 KB buffers
SoundSystem::SoundSystem() : center(QPoint(0,0))
{
    active=false;
}

SoundSystem::~SoundSystem(){
    if (!active) return;
    alcDestroyContext(context);
    alcCloseDevice(device);
    active = false;
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
        if (!sourceStatus(sources.at(i).first)) {
            deleteSource(sources.at(i));
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

void SoundSystem::pauseSource(unsigned int source) {
    if(!active) return;
    alSourcePause(source);
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

void SoundSystem::playOGG(char* name){
    ALint state;                // The state of the sound source
    ALuint bufferID;            // The OpenAL sound buffer ID
    //ALuint sourceID;            // The OpenAL sound source
    ALenum format;              // The sound data format
    ALsizei freq;               // The frequency of the sound data

    std::vector < char > bufferData; // The sound buffer data from file

    // Create sound buffer and source
    alGenBuffers(1, &bufferID);
    alGenSources(1, &sourceID);

    int endian = 0;             // 0 for Little-Endian, 1 for Big-Endian
    int bitStream;
    long bytes;
    char array[BUFFER_SIZE];    // Local fixed size array
    FILE *f = NULL;

    // Open for binary reading
#ifdef __WIN32__
    if( (f = fopen(name, "rb")) == NULL){
#else
    if( (f = fopen(name, "r")) == NULL){
#endif
            qWarning()<<"file "<<name<<" not opened.";
            return;
        }

    vorbis_info *pInfo;
    OggVorbis_File oggFile;

    if(ov_open_callbacks(f, &oggFile, NULL, 0, OV_CALLBACKS_NOCLOSE) < 0) {
        qWarning() << "Input does not appear to be an Ogg bitstream.\n";
        return;
    }

    pInfo = ov_info(&oggFile, -1);

    // Check the number of channels... always use 16-bit samples
    if (pInfo->channels == 1)
        format = AL_FORMAT_MONO16;
    else
        format = AL_FORMAT_STEREO16;
    // end if

    // The frequency of the sampling rate
    freq = pInfo->rate;

    do {
        // Read up to a buffer's worth of decoded sound data
        bytes = ov_read(&oggFile, array, BUFFER_SIZE, endian, 2, 1, &bitStream);
        // Append to end of buffer
        bufferData.insert(bufferData.end(), array, array + bytes);
    } while (bytes > 0);
    ov_clear(&oggFile);

    // Upload sound data to buffer
      alBufferData(bufferID, format, &bufferData[0], static_cast < ALsizei > (bufferData.size()), freq);

      // Attach sound buffer to source
      alSourcei(sourceID, AL_BUFFER, bufferID);

      alSourcef(sourceID,AL_GAIN,0.5f);
      // Finally, play the sound!!!
      alSourcePlay(sourceID);
      // This is a busy wait loop but should be good enough for example purpose
      do {
        // Query the state of the souce
        alGetSourcei(sourceID, AL_SOURCE_STATE, &state);
      } while (state != AL_STOPPED);


      // Clean up sound buffer and source
      alDeleteBuffers(1, &bufferID);
      alDeleteSources(1, &sourceID);
}

ALuint SoundSystem::getSource() {
    //Gets the active source played with playOGG.
    return sourceID;
}
