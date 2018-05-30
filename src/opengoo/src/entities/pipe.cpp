#include <cassert>

#include <QtMath>

#include "GameConfiguration/wog_pipe.h"
#include "GameEngine/og_gameengine.h"

#include "multisprite.h"

#include "pipe.h"

static const char* IMAGE_GLOBAL_PIPE = "IMAGE_GLOBAL_PIPE_";

class CapBuilder
{
public:
    OGSpritePtr Build()
    {
        assert(!mType.isEmpty() && "Build. Type is emtpy");
        auto v = mVertex[0] - mVertex[1];
        auto angle = -(90 + qRadiansToDegrees(qAtan2(v.y(), v.x())));
        auto src = GE->GetResourceManager()->GetImageSourceById(QLatin1String(IMAGE_GLOBAL_PIPE) + QLatin1String("CAP_") + mType);
        auto spr = OGSprite::Create(src);
        spr->CenterOrigin();
        spr->SetAngle(angle);
        return spr;
    }

    CapBuilder& SetType(const QString& aType)
    {
        mType = aType;
        return *this;
    }

    CapBuilder& SetVertices(const QPointF& aVertex1, const QPointF& aVertex2)
    {
        mVertex[0] = aVertex1;
        mVertex[1] = aVertex2;
        return *this;
    }

private:
    QString mType;
    QPointF mVertex[2];
};

static inline QString BuildPrefix(const QPointF& a_p1, const QPointF& a_p2, const QPointF& a_p3)
{
    auto v1 = a_p2 - a_p1;
    auto v2 = a_p3 - a_p2;

    if (v1.y() > 0 || v2.y() < 0)
    {
        return (v2.x() < 0 || v1.x() > 0 ? QLatin1String("TR_") : QLatin1String("TL_"));
    }
    else
    {
        return (v2.x() < 0 || v1.x() > 0 ? QLatin1String("BR_") : QLatin1String("BL_"));
    }
}

class BendBuilder
{
public:
    OGSpritePtr Build()
    {
        assert(!mType.isEmpty() && "Build. Type is emtpy");
        const auto prefix = BuildPrefix(mVertex[0], mVertex[1], mVertex[2]);
        auto src = GE->GetResourceManager()->GetImageSourceById(QLatin1String(IMAGE_GLOBAL_PIPE) + QLatin1String("BEND_") + prefix + mType);

        auto spr = OGSprite::Create(src);
        spr->CenterOrigin();
        spr->SetPosition(mVertex[1].x(), -mVertex[1].y());

        return spr;
    }

    BendBuilder& SetType(const QString& aType)
    {
        mType = aType;
        return *this;
    }

    BendBuilder& SetVertices(const QPointF& aVertex1, const QPointF& aVertex2, const QPointF& aVertex3)
    {
        mVertex[0] = aVertex1;
        mVertex[1] = aVertex2;
        mVertex[2] = aVertex3;
        return *this;
    }

private:
    QString mType;
    QPointF mVertex[3];
};

struct PipeBuilder
{
    PipeBuilder()
        : mShouldUpdate(false)
    {
    }

    OGSpritePtr Build()
    {
        assert(!mType.isEmpty() && "Build. Type is emtpy");
        if (mShouldUpdate)
        {
            mSource = GE->GetResourceManager()->GetImageSourceById(QLatin1String(QLatin1String(IMAGE_GLOBAL_PIPE)) + mType);
            mWidth = mSource->GetWidth();
            mOffset = mSource->GetHeight() * 0.5;
            mShouldUpdate = false;
        }

        auto spr = OGSprite::Create(mSource);
        spr->SetOffsetY(mOffset);

        auto v =  mVertex[1] - mVertex[0];
        spr->SetPosition(mVertex[0].x(), -mVertex[0].y());

        auto sx = std::floor(v.x()) != 0 ? std::fabs(v.x() / mWidth) : std::fabs(v.y() / mWidth);
        spr->SetScaleX(sx);

        auto angle = qRadiansToDegrees(std::atan2(v.y(), v.x()));
        spr->SetAngle(-angle);

        return spr;
    }

    PipeBuilder& SetVertices(const QPointF& aVertex1, const QPointF& aVertex2)
    {
        mVertex[0] = aVertex1;
        mVertex[1] = aVertex2;
        return *this;
    }

    PipeBuilder& SetType(const QString& aType)
    {
        if (aType == mType)
        {
            return *this;
        }

        mShouldUpdate = true;
        mType = aType;
        return *this;
    }

public:
    QString mType;
    QPointF mVertex[2];
    og::ImageSourceSPtr mSource;
    bool mShouldUpdate;
    float mOffset;
    int mWidth;
};

Pipe::Pipe(const WOGPipe& aDef)
    : Entity(aDef.vertex.front().x, -aDef.vertex.front().y)
{
    SetDepth(aDef.depth);

    QString type = aDef.type.isEmpty() ? QLatin1String("NORMAL") : aDef.type;
    const auto& vertex = aDef.vertex;

    auto pipe = std::make_shared<MultiSprite>();
    {
        PipeBuilder builder;
        builder.SetType(type);

        for (int i = 1; i < vertex.size(); ++i)
        {
            QPoint p1(vertex[i - 1].x, vertex[i - 1].y);
            QPoint p2(vertex[i].x, vertex[i].y);
            pipe->AddSprite(builder.SetVertices(p1, p2).Build());
        }
    }

    {
        BendBuilder builder;
        builder.SetType(type);

        for (int i = 2; i < vertex.size(); ++i)
        {
            QPoint p1(vertex[i - 2].x, vertex[i - 2].y);
            QPoint p2(vertex[i - 1].x, vertex[i - 1].y);
            QPoint p3(vertex[i].x, vertex[i].y);
            pipe->AddSprite((builder.SetVertices(p1, p2, p3).Build()));
        }
    }

    mPipe = pipe;

    CapBuilder builder;
    QPoint p1(aDef.vertex[0].x, aDef.vertex[0].y);
    QPoint p2(aDef.vertex[1].x,aDef.vertex[1].y);
    builder.SetVertices(p1, p2);

    builder.SetType(QLatin1String("OPEN_") + type);
    mCap[e_open] = builder.Build();

    builder.SetType(QLatin1String("CLOSED_") + type);
    mCap[e_closed] = builder.Build();

    SetGraphic(mCap[e_closed]);
}

void Pipe::Render(QPainter& aPainter)
{
    mPipe->Render(aPainter, GetPosition());
    Entity::Render(aPainter);
}

void Pipe::OnOpen()
{
    SetGraphic(mCap[e_open]);
}

void Pipe::OnClosed()
{
    SetGraphic(mCap[e_closed]);
}
