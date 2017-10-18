#pragma once

#include <QObject>

class GameMenu : public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY(GameMenu)

public:
    GameMenu();
    ~GameMenu();

signals:
    void close();
    void backToIsland();

private:
    struct Impl;
    std::unique_ptr<Impl> mImpl;

private slots:
    void restart();
    void ocdcriteria();
};
