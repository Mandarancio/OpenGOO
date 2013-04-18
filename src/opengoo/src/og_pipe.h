#ifndef OG_PIPE_H
#define OG_PIPE_H

#include "og_ipipe.h"

#include <QPointF>
#include <QString>

struct WOGPipe;

class WOGResources;

struct OGSprite;

class OGWorld;

class QPainter;
class QImage;

class OGPipe : public OGIPipe
{
        OGSprite* pCapClosed_;
        OGSprite* pCapOpen_;

        virtual void _Close();
        virtual void _Open();
        virtual void _Painter(QPainter* painter) { Q_UNUSED(painter); }

        OGWorld* _GetWorld();
        QImage* _GetImage(const QString &id);
        void _InsertSprite(OGSprite* sprite);
        WOGResources* _GetResourceManager();
        OGSprite* _CreateCap(WOGPipe* pipe, const QString &id, bool visible);
        OGSprite* _CreateBend(const QString &type, const QPointF &p1
                              , const QPointF &p2, const QPointF &p3
                              , float depth);

    public:
        OGPipe(WOGPipe* pipe);
};

#endif // OG_PIPE_H
