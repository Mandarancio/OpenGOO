#pragma once

#include <QObject>
#include <memory>

class QString;

class Island : public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY(Island)

    public:
        Island(const QString &);
        ~Island();

    signals:
        void close();

    private:
        struct Impl;
        std::unique_ptr<Impl> _pImpl;
};
