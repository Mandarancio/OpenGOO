#ifndef OPENGOO_H
#define OPENGOO_H

#include "GameEngine/og_game.h"

#include <QString>

class OGWorld;
class OGFPSCounter;

class OpenGOO : public OGGame
{
    OpenGOO() {}
    ~OpenGOO() {}

    static OpenGOO* instance_;

    OGWorld* pWorld_;
    OGFPSCounter* pFPS_;

    QString levelName_;
    QString language_;

    int width_;
    int height_;

    float timeStep_;
    float timeScrollStep_;

    void _Start();
    void _End();

    void _LoadLevel(const QString& levelname);
    void _CreateUIButtons();
    void _LoadMainMenu();

public:
    static OpenGOO* instance();

    void Destroy();

    void SetLevelName(const QString& levelname);
    void SetLanguage(const QString& language);
};

#endif // OPENGOO_H
