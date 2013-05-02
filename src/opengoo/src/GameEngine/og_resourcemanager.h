#ifndef OG_RESOURCEMANAGER_H
#define OG_RESOURCEMANAGER_H

class WOGResources;

class QImage;
class QString;

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

#endif // OG_RESOURCEMANAGER_H
