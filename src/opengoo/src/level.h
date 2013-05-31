#ifndef LEVEL_H
#define LEVEL_H

#include <QObject>

class QString;

class Level : public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY(Level)

public:
    Level(const QString &levelname);
    ~Level();

    void reload();

    void hideButton();
    void showButton();

signals:
    void closeLevel();

private:
    struct Impl;
    std::unique_ptr<Impl> _pImpl;

private slots:
   void _retryButton();
   void _menuButton();

   void _retryMenu();
   void _gameMenu();
};

#endif // LEVEL_H
