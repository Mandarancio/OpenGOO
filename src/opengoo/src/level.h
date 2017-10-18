#pragma once

#include <QObject>

class Level : public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY(Level)

public:
    Level(const QString& levelname);
    ~Level();

    void reload();

    void hideButton();
    void showButton();

signals:
    void closeLevel();

private:
    struct Impl;
    std::unique_ptr<Impl> mImpl;

private slots:
   void retryButton();
   void menuButton();

   void retryMenu();
   void gameMenu();
};
