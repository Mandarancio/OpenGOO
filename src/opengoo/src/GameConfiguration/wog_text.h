#ifndef WOG_TEXT_H
#define WOG_TEXT_H

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
};

#endif // WOG_TEXT_H
