#ifndef OG_RESOURCEMANAGER_H
#define OG_RESOURCEMANAGER_H

class WOGResources;

class QImage;
class QString;

namespace og
{
class OGResourceManager
{
public:
    OGResourceManager();
    ~OGResourceManager();

    bool ParseResourceFile(const QString &filename);

    QImage* GetImage(const QString &id);

private:
    WOGResources* src_;
};
} // namespace og

#endif // OG_RESOURCEMANAGER_H
