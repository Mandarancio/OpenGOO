#pragma once

class EntityFactory;
struct WOGPipe;
struct WOGLevelExit;

struct GameData
{
    EntityFactory* efactory;
    WOGPipe* pipe;
    WOGLevelExit* levelexit;
    int ballsrequired;
};

