#include "og_wavfile.h"

#include <cstring>

void OGWavFile::Open(const OGpchar filename)
{
    isFailbit_ = true;
    ifs_.open(filename, std::fstream::binary);

    if (!ifs_.fail())
    {
        wavInfo_ = new WaveInfo;
        ifs_.seekg (0, std::ios::beg);
        ifs_.read ((char *)wavInfo_, sizeof(WaveInfo));

        if (IsWavFile())
        {
            isFailbit_ = false;
            this->GetInfo();
            return;
        }
    }

    ifs_.close();
}

void OGWavFile::Close()
{
    if (ifs_.is_open())
    {
        delete wavInfo_;
        ifs_.close();
    }
}

OGpchar OGWavFile::ReadData()
{
    if (dataSize_ > 0)
    {
        OGpchar buffer = new char[dataSize_];
// Sample data
        ifs_.seekg (sizeof(WaveInfo), std::ios::beg);
        ifs_.read (buffer, dataSize_);

        return buffer;
    }
    else { return 0; }
}

bool OGWavFile::IsWavFile()
{
// Chunk ID	"RIFF" (0x52494646)
    if (strncmp(wavInfo_->riffChunck.id, "RIFF", sizeof(wavInfo_->riffChunck.id)) ==  0)
    {
// RIFF Type "WAVE" (0x57415645)
        if (strncmp(wavInfo_->riffChunck.type, "WAVE", sizeof(wavInfo_->riffChunck.type)) ==  0)
        { return true; }
    }

    return false;
}

void OGWavFile::GetInfo()
{
// Chunk ID	"fmt " (0x666D7420)
    if (strncmp(wavInfo_->fmt.id, "fmt ", sizeof(wavInfo_->fmt.id)) ==  0)
    {
// Number of channels
        channels_ = wavInfo_->fmt.channels;
// Sample rate
        sampleRate_ = wavInfo_->fmt.samplesPerSec;
    }
    else
    {
        channels_ = 0;
        sampleRate_ = 0;
    }

// Chunk ID	"data" (0x64617461)
    if (strncmp(wavInfo_->data.id, "data", sizeof(wavInfo_->data.id)) ==  0)
    {
// Data size
        dataSize_ = wavInfo_->data.size;
    }
    else { dataSize_ = 0; }
}
