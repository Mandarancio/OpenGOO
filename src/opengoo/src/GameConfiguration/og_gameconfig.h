#ifndef OG_GAMECONFIG_H
#define OG_GAMECONFIG_H

#include "og_xmlconfig.h"

struct OGConfig
{
    int screen_width;
    int screen_height;
    bool fullscreen;
    QString language;    
};

class OGGameConfig : public OGXmlConfig
{
public:
    OGGameConfig(const QString & filename);
    OGConfig Parser();
    void Create(OGConfig & config); // Create new game configuration file
};

#endif // OG_GAMECONFIG_H
