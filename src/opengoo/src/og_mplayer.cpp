#include "og_mplayer.h"

#include <vector>

#include <OGAlut>
#include <OGOggFile>

#include <QDebug>

OGMPlayer::OGMPlayer(QObject* parent)
    : QThread(parent)
{
    abort_ = false;
}

OGMPlayer::~OGMPlayer()
{
    Close();
}

void OGMPlayer::Open(const OGpchar filename, bool loopmode, bool streammode)
{
    fileName_ = filename;

    isLoopMode_ = loopmode;
    isStreamMode_ = streammode;

    source_ = OGAlut::CreateSource();

    if(!isStreamMode_)
    {
        buffer_ = OGAlut::CreateBufferFromFile(fileName_);
        OGAlut::AttachBuffer(source_, buffer_);
        if (isLoopMode_) { alSourcei(source_, AL_LOOPING, AL_TRUE); }
    }
}

void OGMPlayer::run()
{
    int state;
    int processed;
    OGuint buffers;
    oggFile_ = new OGOggFile;

    forever
    {
        CreateBuffer();
//        mutex_.lock();
        alSourcePlay(source_);
//        mutex_.unlock();

        do
        {
            // Query the state of the souce
//            mutex_.lock();
            state = OGAlut::GetState (source_);
//            mutex_.unlock();
            if (state == AL_PAUSED)
            {
                condition_.wait(&mutex_);
            }

            alGetSourcei(source_, AL_BUFFERS_PROCESSED, &processed);
            while (processed)
            {
                alSourceUnqueueBuffers(source_, 1, &buffers);
                OGAlut::DeleteBuffer(buffers);
                buffers_.pop();
                processed--;
                Update();
            }

            msleep(100);
        }
        while (state != AL_STOPPED);

        if (abort_)
        {
            alSourcei(source_, AL_BUFFER, 0);

            for(int i=0; i<(int)buffers_.size(); i++)
            {
                OGAlut::DeleteBuffer(buffers_.front());
                buffers_.pop();
            }

            delete oggFile_;

            return;
        }

        if (!isLoopMode_) { return; }
        else { oggFile_->Close(); }
    }
}

void OGMPlayer::CreateBuffer()
{
    oggFile_->Open(fileName_);

    if (oggFile_->IsFail()) { return; }

    if (oggFile_->Channels() == 1)
        format_ = AL_FORMAT_MONO16;
    else
        format_ = AL_FORMAT_STEREO16;

    freq_ = oggFile_->SampleRate();

    for (int i=0; i < 5; i++)
    {
        Update();
    }
}

void OGMPlayer::Update()
{
    if (oggFile_->IsEOF()) { return; }

    const int BUFFER_SIZE = 4096;
    std::vector<char> buffer;
    char data[BUFFER_SIZE];

    for (int i=0; i < 10; i++) {
        oggFile_->ReadData(data, BUFFER_SIZE);

        if (oggFile_->DataSize() > 0)
        {
                buffer.insert(buffer.end(), data, data + oggFile_->DataSize());
        }
    }

    OGuint bufferID = OGAlut::CreateBuffer();
    alBufferData(bufferID, format_, &buffer[0], buffer.size(), freq_);
    alSourceQueueBuffers(source_, 1, &bufferID);
    buffers_.push(bufferID);
    if (oggFile_->IsEOF()) { oggFile_->Close(); }
}

void OGMPlayer::Play()
{   
    if (isStreamMode_)
    {
        if (!isRunning()) { start(LowPriority); }
        else
        {
            alSourcePlay(source_);
            condition_.wakeOne();
        }
    }
    else { alSourcePlay(source_); }
}

void OGMPlayer::Pause()
{
    if (isStreamMode_)
    {
        if (isRunning()) { alSourcePause(source_); }
    }
    else { alSourcePause(source_); }
}

void OGMPlayer::Stop()
{
    if (isStreamMode_)
    {
        if (isRunning())
        {
            mutex_.lock();
            abort_ = true;
            condition_.wakeOne();
            alSourceStop(source_);
            mutex_.unlock();
            wait();
        }
    }
    else { alSourceStop(source_); }
}

void OGMPlayer::Close()
{
    if (isStreamMode_) {
        Stop();
    }
    else {
        OGAlut::DeleteSource(source_);
        OGAlut::DeleteBuffer(buffer_);
    }
}
