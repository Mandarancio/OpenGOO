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
        virtual void initNewTarget(){}
    protected:
        QVector2D position;
        float speed;
    private:
        virtual void onTargetChanged() {}
};

class OGIClimbBehavior : public OGBehavior
{
    public:
        virtual ~OGIClimbBehavior() {}
    void SetOrigin(float posx, float posy);
    void SetOrigin(const QPointF &pos);
    void SetOrigin(const QVector2D &pos);
    void Climb() { _Climb(); }
    protected:
        QVector2D origin;
    private:
        virtual void onOriginChanged() {}
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
