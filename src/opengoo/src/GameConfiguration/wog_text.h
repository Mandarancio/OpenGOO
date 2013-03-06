#ifndef WOG_TEXT_H
#define WOG_TEXT_H

#include <QString>
#include <QList>

struct WOGString
{
    QString id;
    QString text;
};

struct WOGText
{
    QString language;
    QList<WOGString*> string;

    ~WOGText() { while (!string.isEmpty()) { delete string.takeFirst(); } }

    QString GetString(const QString& str);
};

#endif // WOG_TEXT_H
