#ifndef OG_IFILE_H
#define OG_IFILE_H

#include <OGTypes>

class OGIFile
{
public:
    virtual ~OGIFile() {}
    virtual void Open(const OGpchar filename) = 0;
    virtual void Close() = 0;
    virtual OGpchar ReadData() = 0;

    bool IsFail() const { return isFailbit_; }

    int Channels() const { return channels_; }
    int SampleRate() const { return sampleRate_; }
    int DataSize() const { return dataSize_; }


protected:
    bool isFailbit_;
    int channels_;
    int sampleRate_;
    int dataSize_;

    virtual void GetInfo() = 0;

};

#endif // OG_IFILE_H
