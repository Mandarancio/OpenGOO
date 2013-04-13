#ifndef OG_PIPE_H
#define OG_PIPE_H

#include "og_ipipe.h"

#include <QString>

struct WOGPipe;

class WOGResources;

struct OGSprite;

class OGWorld;

class QPainter;
class QImage;

class OGPipe : public OGIPipe
{
        virtual void _Close() {}
        virtual void _Open() {}
        virtual void _Painter(QPainter* painter) { Q_UNUSED(painter); }

        OGWorld* _GetWorld();
        QImage* _GetImage(const QString &id);
        void _InsertSprite(OGSprite* sprite);
        WOGResources* _GetResourceManager();

    public:
        OGPipe(WOGPipe* pipe);
};

#endif // OG_PIPE_H
