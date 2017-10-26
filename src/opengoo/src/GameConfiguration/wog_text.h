#pragma once

#include <QString>
#include <QHash>

struct WOGText
{
    QString language;
    QHash<QString, QString> string;
};
