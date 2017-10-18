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

bool loadConfig(OGConfig& config, const QString & filename);
void saveConfig(OGConfig& config, const QString & filename);

inline OGUserData* getUserData(void* userdata)
{
    return userdata ? static_cast<OGUserData*>(userdata) : nullptr;
}

QPixmap* getImage(const QString& id); // TODO Remove
QString getText(const QString&id); // TODO Remove

std::unique_ptr<UIData> getUIData(const QString &id);

template<class T> std::unique_ptr<T> createUI(const QPoint& pos, const UIData& data)
{
    auto ui = std::unique_ptr<T>(new T);

    ui->setPosition(pos.x(), pos.y());
    ui->setSize(data.width, data.height);
    ui->setUpImage(getImage(data.up));
    ui->setOverImage(getImage(data.over));
    ui->setText(getText(data.text));

    return ui;
}

std::unique_ptr<og::ui::PushButton> createPushButton(const QPoint &pos, const UIData &data);

template<typename T, size_t N> inline size_t array_size(T (&)[N])
{
    return N;
}

} // ns:utils
} // ns:og
