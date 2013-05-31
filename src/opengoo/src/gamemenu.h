#ifndef GAMEMENU_H
#define GAMEMENU_H

#include <QObject>

#include <OGPushButton>

struct WOGButton;
class WOGResources;

class QPixmap;

class GameMenu : public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY(GameMenu)

public:
    GameMenu();

signals:
    void close();
    void backToIsland();

private:
    struct Impl;
    std::unique_ptr<Impl> _pImpl;

    std::unique_ptr<og::ui::PushButton> _createButton(const WOGButton* button
                                      , const WOGResources &resrc);
    QPixmap* _getImage(const QString &id, const WOGResources &resrc);

private slots:
    void _restart();
    void _ocdcriteria();
};

#endif // GAMEMENU_H
