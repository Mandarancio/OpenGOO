#pragma once

#include <QHash>
#include <QString>

#include "wog_ball.h"
#include "imagesource.h"

class WOGResources;

namespace og
{
class OGResourceManager
{
    typedef std::unique_ptr<WOGBall> WOGBallPtr;
    typedef std::shared_ptr<WOGBall> WOGBallSPtr;
    typedef std::shared_ptr<ImageSource> ImageSourceSPtr;
    typedef std::shared_ptr<WOGResources> WOGResourcesSPtr;

public:
    OGResourceManager();
    ~OGResourceManager();

    bool ParseResourceFile(const QString& a_filename);

    WOGBall* GetBallByType(const QString& a_type);

    ImageSourceSPtr GetImageSourceById(const QString& a_id);

private:
    template<typename T>
    bool Load(T& a_ball, const QString& a_filename);

private:
    QHash<QString, WOGResourcesSPtr> m_resources;
    QHash<QString, WOGBallSPtr> m_balls;
    QHash<QString, ImageSourceSPtr> m_imageSources;
};
}
