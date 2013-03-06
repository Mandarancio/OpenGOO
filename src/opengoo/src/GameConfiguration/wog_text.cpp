#include "wog_text.h"

QString WOGText::GetString(const QString& str)
{
    Q_FOREACH (WOGString*s, string)
    {
        if (s->id.compare(str) == 0)
        {
            return s->text;
        }
    }

    return QString();
}
