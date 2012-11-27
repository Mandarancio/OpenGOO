#include "og_alut.h"

#include <vector>
#include <cstdio>

#include <vorbis/vorbisfile.h>

#include <QFileInfo>
#include <QDebug>

#include <OGWavFile>
#include <OGOggFile>

bool OGAlut::Init()
{
    ALCdevice *device;
    ALCcontext *context;

    /* Open and initialize a device with default settings */
    device = alcOpenDevice(0);

    if (!device) { return false; }

    context = alcCreateContext(device, 0);

    if (context == 0 || alcMakeContextCurrent(context) == ALC_FALSE)
    {
        if (context != 0) { alcDestroyContext(context); }

        alcCloseDevice(device);

        return false;
    }

    return true;
}

bool OGAlut::Exit()
{
    ALCdevice *device;
    ALCcontext *context;

    /* Close the device belonging to the current context, and destroy the
     * context. */
    context = alcGetCurrentContext();

    if (context == 0) { return false; }

    device = alcGetContextsDevice(context);
    alcMakeContextCurrent(0);
    alcDestroyContext(context);
    alcCloseDevice(device);

    return true;
}

OGuint OGAlut::CreateBufferFromFile (const OGpchar filename)
{
    OGuint buffer;
    QFileInfo fi(filename);
    QString ext = fi.suffix();

    if (ext.compare("ogg", Qt::CaseInsensitive) == 0)
    {
        buffer = CreateBufferFromOgg(filename);
    }
    else if (ext.compare("wav", Qt::CaseInsensitive) == 0)
    {
        buffer = CreateBufferFromWav(filename);
    }
    else return 0;

    return buffer;
}

OGuint OGAlut::CreateBufferFromWav(const OGpchar filename)
{
    OGWavFile wavFile(filename);

    if (wavFile.IsFail()) { return -1; }

    OGuint buffer = CreateBuffer();            // The OpenAL sound buffer ID

    ALenum format;              // The sound data format

    // Check the number of channels... always use 16-bit samples
    if (wavFile.Channels() == 1) { format = AL_FORMAT_MONO16; }
    else { format = AL_FORMAT_STEREO16; }

    alBufferData(buffer, format, wavFile.ReadData(),  wavFile.DataSize(), wavFile.SampleRate());

    return buffer;
}

OGuint OGAlut::CreateBufferFromOgg(const OGpchar filename)
{
    OGOggFile oggFile(filename);

    if (oggFile.IsFail()) { return -1; }

    OGuint bufferID = CreateBuffer();            // The OpenAL sound buffer ID

    ALenum format;              // The sound data format

    // Check the number of channels... always use 16-bit samples
    if (oggFile.Channels() == 1) { format = AL_FORMAT_MONO16; }
    else { format = AL_FORMAT_STEREO16; }

    OGpchar data = oggFile.ReadData();
    alBufferData(bufferID, format, data,  oggFile.DataSize(), oggFile.SampleRate());

    return bufferID;
}

OGuint OGAlut::CreateBuffer()
{
    OGuint buffer;

    alGetError(); // clear error code
    alGenBuffers(1, &buffer);

    if (alGetError() != AL_NO_ERROR) { return -1; }
    else { return buffer; }
}

OGuint OGAlut::CreateSource()
{
    OGuint source;

    alGetError(); // clear error code
    alGenSources(1, &source);

    if (alGetError() != AL_NO_ERROR) { return -1; }
    else { return source; }
}

bool OGAlut::AttachBuffer(OGuint source, OGuint buffer)
{
    alGetError(); // clear error code
    alSourcei(source, AL_BUFFER, buffer);

    if (alGetError() != AL_NO_ERROR) { return false; }
    else { return true; }
}

int OGAlut::GetState(OGuint source)
{
    int state;
    alGetSourcei(source, AL_SOURCE_STATE, &state);

    return state;
}

void OGAlut::SetVolume(OGuint source, float volume)
{
    alSourcef(source, AL_GAIN, volume);
}

void OGAlut::SetPitch(OGuint source, float pitch)
{
    alSourcef(source, AL_PITCH, pitch);
}

void OGAlut::SetPosition(OGuint source, float x, float y)
{
    alSource3f(source, AL_POSITION, x, y, 0);
}
