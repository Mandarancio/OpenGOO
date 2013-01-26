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

namespace{
//delete warning about unused variables.
ov_callbacks un_1 = OV_CALLBACKS_DEFAULT;
ov_callbacks un_2 = OV_CALLBACKS_NOCLOSE;
ov_callbacks un_3 = OV_CALLBACKS_STREAMONLY;
ov_callbacks un_4 = OV_CALLBACKS_STREAMONLY_NOCLOSE;
}

#endif // OG_OGGFILE_H
