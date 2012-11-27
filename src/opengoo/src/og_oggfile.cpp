#include "og_oggfile.h"

#include <stdio.h>

#include <QDebug>

#include <soundsystem.h>
#include <OGAlut>

void OGOggFile::Open(const OGpchar filename)
{
    if (ov_fopen(filename, &vorbisFile_) < 0)
    {
        isOpen_ = false;
        isFailbit_ = true;
        return;
    }
    else
    {
        isOpen_ = true;
        isFailbit_ = false;
        GetInfo();
        dataSize_= 0;
        isEOF_ = false;
    }
}

void OGOggFile::Close()
{
    if (isOpen_)
    {
        ov_clear(&vorbisFile_);
    }
}

void OGOggFile::GetInfo()
{
    vorbis_info* pInfo = ov_info(&vorbisFile_, -1);
    channels_ = pInfo->channels;
    sampleRate_ = pInfo->rate;
}

OGpchar OGOggFile::ReadData()
{
    if (!isOpen_) { return 0; }
    long bytes;
    const int BUFFER_SIZE = 4096;
    char vorbisfileBuffer[BUFFER_SIZE];    // Local fixed size array
    int bitStream;

    while (!isEOF_)
    {
        // Read up to a buffer's worth of decoded sound data
        bytes = ov_read(&vorbisFile_, vorbisfileBuffer, BUFFER_SIZE, 0, 2, 1, &bitStream);
        if (bytes == 0) { isEOF_ = true; }
        else if ( bytes < 0) { isFailbit_ = true; }
        else
        {
        // Append to end of buffer
        bufferData_.insert(bufferData_.end(), vorbisfileBuffer, vorbisfileBuffer + bytes);
        }
    }

    dataSize_ = bufferData_.size();

    return &bufferData_[0];
}

void OGOggFile::ReadData(OGpchar buffer, int buffersize)
{
    if (!isOpen_) { return; }
    long bytes;
    int bitStream;

    bytes = ov_read(&vorbisFile_, buffer, buffersize, 0, 2, 1, &bitStream);
    if (bytes == 0) { isEOF_ = true; }
    else if ( bytes < 0) { isFailbit_ = true; }
    dataSize_ = bytes;
}
