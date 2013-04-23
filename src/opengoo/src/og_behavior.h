#ifndef OG_BEHAVIOR_H
#define OG_BEHAVIOR_H

#include <QVector2D>

class QPointF;

class OGBehavior
{
    public:
        virtual ~OGBehavior() {}
        void SetTarget(float posx, float posy);
        void SetTarget(const QPointF &pos);
        void SetTarget(const QVector2D &pos);
        void SetSpeed(float s);

    protected:
        QVector2D position;
        float speed;
};

class OGIClimbBehavior : public OGBehavior
{
    public:
        virtual ~OGIClimbBehavior() {}
        void Climb() { _Climb(); }

    private:
        virtual void _Climb() = 0;
};

class OGIFlyBehavior : public OGBehavior
{
    public:
        virtual ~OGIFlyBehavior() {}
        void Fly() { _Fly(); }

    private:
        virtual void _Fly() = 0;
};

class OGIWalkBehavior : public OGBehavior
{
    public:
        virtual ~OGIWalkBehavior() {}
        void Walk() { _Walk(); }

    private:
        virtual void _Walk() = 0;
};

#endif // OG_BEHAVIOR_H
