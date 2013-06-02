#ifndef ISLAND_H
#define ISLAND_H

#include <QObject>
#include <memory>

class QString;

class Island : public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY(Island)

    public:
        Island(const QString &name);
        ~Island();

    signals:
        void close();

    private:
        struct Impl;
        std::unique_ptr<Impl> _pImpl;
};

#endif // ISLAND_H
