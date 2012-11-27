#ifndef OG_OGGFILE_H
#define OG_OGGFILE_H

#include <vector>

#include <vorbis/vorbisfile.h>

#include <OGIFile>

class OGOggFile : public OGIFile
{
public:
    OGOggFile() {}
    OGOggFile(const OGpchar filename) { Open(filename); }
    ~OGOggFile() { Close(); }

    void Open(const OGpchar filename);
    void Close();

    OGpchar ReadData();
    void ReadData(OGpchar buffer, int buffersize);

    bool IsEOF() const { return isEOF_; }

private:
    OggVorbis_File vorbisFile_;
    bool isEOF_;
    bool isOpen_;

    std::vector < char > bufferData_; // The sound buffer data from file


    void GetInfo();

};

#endif // OG_OGGFILE_H
