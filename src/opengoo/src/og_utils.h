#ifndef OG_UTILS_H
#define OG_UTILS_H

#include "og_gameconfig.h"
#include <consoleappender.h>

#include "OGLib/circlef.h"

#include <OGPushButton>
#include "GameEngine/og_gameengine.h"
#include "uidata.h"
#include "opengoo.h"

struct OGUserData;

class OGContactListener;

namespace ogUtils
{

void ogBackTracer();

void ogLogger();

void ogMessageHandler(QtMsgType, const QMessageLogContext &, const QString&);

bool ogLoadConfig(OGConfig& config, const QString & filename);
void ogSaveConfig(OGConfig& config, const QString & filename);

OGUserData* ogGetUserData(void* userdata);

OGContactListener* ogGetContactListener();

QPixmap* getImage(const QString & id);
QString getText(const QString &id);

og::OGGameEngine* getGameEngine();

std::unique_ptr<UIData> getUIData(const QString &id);

template<class T> T* createUI(const QPoint &pos, const UIData &data);
og::ui::PushButton* createButton(const QPoint &pos, const UIData &data);

OpenGOO* getGame();
}

inline OpenGOO* ogUtils::getGame() { return OpenGOO::instance(); }

inline og::OGGameEngine* ogUtils::getGameEngine()
{
    return og::OGGameEngine::getInstance();
}

#endif // OG_UTILS_H
