#pragma once

struct WOGImage
{
    QString id;
    QPointF imagepos;
    QPointF imagescale;
    float imagerot;

    WOGImage()
        : imagerot(0)
    {
    }

    bool IsEmpty() const
    {
        return id.isEmpty();
    }
};

