#include "og_resourcemanager.h"
#include "og_resourceconfig.h"
#include "wog_resources.h"
#include "og_data.h"

#include <QFile>
#include <QImage>
#include <QString>

using namespace og;

OGResourceManager::OGResourceManager() : src_(0)
{    
}

OGResourceManager::~OGResourceManager()
{
    delete src_;
}

bool OGResourceManager::ParseResourceFile(const QString &filename)
{           
    if (QFile::exists(filename))
    {
        src_ = OGData::GetResources(filename);

        if (src_) return true;
    }

    return false;
}

QImage* OGResourceManager::GetImage(const QString &id)
{
    return new QImage(src_->GetImage(id));
}
