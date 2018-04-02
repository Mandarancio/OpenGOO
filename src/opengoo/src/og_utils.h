#pragma once

#include <consoleappender.h>

struct OGUserData;
struct OGConfig;
struct UIData;

namespace og
{
namespace ui
{
class PushButton;
}

namespace utils
{

void backTracer();
void logger();

void messageHandler(QtMsgType, const QMessageLogContext &, const QString&);

bool loadConfig(OGConfig& aConfig, const QString& aFileName);
void saveConfig(OGConfig& config, const QString & aFileName);

inline OGUserData* getUserData(void* userdata)
{
    return userdata ? static_cast<OGUserData*>(userdata) : nullptr;
}

std::unique_ptr<UIData> getUIData(const QString &id);

std::unique_ptr<og::ui::PushButton> createPushButton(const QPoint &pos, const UIData &data);

template<typename T, size_t N> inline size_t array_size(T (&)[N])
{
    return N;
}

} // ns:utils
} // ns:og
