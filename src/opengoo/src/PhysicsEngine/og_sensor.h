#ifndef OG_SENSOR_H
#define OG_SENSOR_H

#include "og_types.h"

#include <QString>
#include <QVector2D>

struct OGSensorFilter
{
    unsigned short category;
    unsigned short mask;
};

class OGSensor
{
    public:
        OGSensor(const QString &id = QString()) : id_(id) {}
        virtual ~OGSensor() {}

        const QString &id() const { return id_; }

        Fixture* GetFixture() const { return _GetFixture(); }
        QVector2D GetPosition() const { return _GetPosition(); }

        void SetCategory(unsigned short category) { _SetCategory(category); }
        void SetMask(unsigned short mask) { _SetMask(mask); }       
        void SetFilter(const OGSensorFilter &filter) { _SetFilter(filter); }

        void BeginContact(Fixture* fixture) { _BeginContact(fixture); }
        void EndContact(Fixture* fixture) { _EndContact(fixture); }

    private:
        QString id_;

        virtual Fixture* _GetFixture() const = 0;
        virtual QVector2D _GetPosition() const = 0;

        virtual void _SetCategory(unsigned short category) = 0;
        virtual void _SetMask(unsigned short mask) = 0;
        virtual void _SetFilter(const OGSensorFilter &filter) = 0;

        virtual void _BeginContact(Fixture* fixture) = 0;
        virtual void _EndContact(Fixture* fixture) = 0;
};

#endif // OG_SENSOR_H
