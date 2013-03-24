#include "og_ibody.h"

OGIBody::OGIBody(WOGPObject* data, WOGMaterial* material)
    : data_(data), material_(material), debug_(false)
{
    walkable_ = false;

    QStringList tags = data_->tag.split(",");

    Q_FOREACH(QString tag, tags)
    {
        if (tag == "walkable") { walkable_ = true; }
    }
}
