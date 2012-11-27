#ifndef OG_MPLAYER_H
#define OG_MPLAYER_H

#include <queue>

#include <vorbis/vorbisfile.h>

#include <QMutex>
#include <QWaitCondition>
#include <QThread>

#include <OGTypes>
#include <OGOggFile>

class OGMPlayer : public QThread
{
    Q_OBJECT

public:
    explicit OGMPlayer(QObject* parent=0);
    ~OGMPlayer();
    void Open(const OGpchar filename, bool loopmode=false, bool streammode=false);

    void Play();
    void Stop();
    void Pause();
    void Close();


signals:

public slots:


protected:
     void run();

private:
    OGuint source_;
    OGuint buffer_;
    std::queue <OGuint> buffers_;

    OggVorbis_File vorbisFile_;
    OGOggFile* oggFile_;
    OGpchar fileName_;
    bool isLoopMode_;
    bool isStreamMode_;
    int format_;
    int freq_;

    QMutex mutex_;
    QWaitCondition condition_;
    bool abort_;

    void CreateBuffer();
    void Update();
};

#endif // OG_MPLAYER_H
