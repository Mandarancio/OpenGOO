#pragma once

struct OGConfig;

class OGApplication
{
public:
    static const int DEFAULT_FRAMERATE = 60;
    static const int DEFAULT_SCREEN_WIDTH = 800;
    static const int DEFAULT_SCREEN_HEIGHT = 600;
    static const char* DEFAULT_LANGUAGE;
    static const char* DEFAULT_LEVEL_NAME;

    static const char* RESOURCES_DIR;
    static const char* PROPERTIES_DIR;
    static const char* FILE_CONFIG;

public:   
    int run(int argc, char** argv);

private:
    bool initialize(int argc, char** argv, OGConfig& config);
};
