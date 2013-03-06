#ifndef OG_DATA_H
#define OG_DATA_H

#include <OGConfiguration>

class OGData
{
        template<class T, class C> static T* _GetData(const QString &path);

    public:
        static WOGLevel* GetLevel(const QString &path);
        static WOGScene* GetScene(const QString &path);
        static WOGText* GetText(const QString &path, const QString &lang);
        static WOGResources* GetResources(const QString &path);
};

#endif // OG_DATA_H
