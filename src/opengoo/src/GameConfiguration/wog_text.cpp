#include "wog_text.h"

QString WOGText::GetString(const QString& str)
{
    QString text;

    Q_FOREACH (WOGString*s, string)
    {
        if (s->id.compare(str) == 0)
        {
            text = s->text;
            break;
        }
    }

    return text;
}
