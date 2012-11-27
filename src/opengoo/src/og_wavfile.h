#ifndef OG_WAVEFILE_H
#define OG_WAVEFILE_H

#include <fstream>

#include <OGIFile>

// Source http://www.sonicspot.com/guide/wavefiles.html
// Wave File Header - RIFF Type Chunk
typedef struct {
    char id[4];               // [4]Chunk ID	"RIFF" (0x52494646)
    OGuint size;              // [4] Chunk Data Size
    char type[4];             // [4] RIFF Type "WAVE" (0x57415645)
} RiffChunk;

// Format Chunk - "fmt "
typedef struct {
    char id[4];               // [4] Chunk ID	"fmt " (0x666D7420)
    OGuint size;              // [4] Chunk Data Size
    OGushort compression;     // [2] Compression code
    OGushort channels;        // [2] Number of channels
    OGuint samplesPerSec;     // [4] Sample rate
    OGuint avgBytesPerSec;    // [4] Average bytes per second
    OGushort blockAlign;      // [2] Block align
    OGushort bitsPerSample;   // [2] Significant bits per sample
//  Extra format bytes does not exist if the uncompressed PCM file
//    OGushort extraSize;         // [2] Extra format bytes
} FormatChunk;

// Data Chunk - "data"
typedef struct {
    char id[4];               // [4] Chunk ID	"data" (0x64617461)
    OGuint size;              // [4] Chunk size
} DataChunk;

typedef struct {
    RiffChunk riffChunck;     // [12] Wave File Header - RIFF Type Chunk
    FormatChunk fmt;          // [24] Format Chunk - "fmt "
    DataChunk data;           // [8] Data Chunk - "data"
} WaveInfo;

class OGWavFile : public OGIFile
{
    public:
    OGWavFile() {}
    OGWavFile(const OGpchar filename) { Open(filename); }
    ~OGWavFile() { Close(); }

    void Open(const OGpchar filename);
    void Close();
    OGpchar ReadData();

    private:
    std::ifstream ifs_;

    WaveInfo* wavInfo_;

    void GetInfo();
    bool IsWavFile();
};

#endif // OG_WAVEFILE_H
