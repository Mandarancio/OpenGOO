#ifndef WOG_BALL_H
#define WOG_BALL_H

#include <QDebug>

struct WOGBall
{
    ~WOGBall()
    {
        qDebug("WOGBall: End");
    }
};

#endif // WOG_BALL_H
