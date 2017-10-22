#pragma once

#include "OGLib/point.h"

namespace oglib
{

class ICamera
{
    public:
        virtual ~ICamera() {}

        float zoom() const { return _GetZoom(); }

        int x() const { return _GetX(); }
        int y() const { return _GetY(); }

        int width() const { return _GetWidth(); }
        int height() const { return _GetHeight(); }

        void SetZoom(float zoom) { _SetZoom(zoom); }

        void MoveUp(int posy) { _MoveUp(posy); }
        void MoveDown(int posy) { _MoveDown(posy); }
        void MoveLeft(int posx) { _MoveLeft(posx); }
        void MoveRight(int posx) { _MoveRight(posx); }
        void MoveTo(int posx, int posy) { _MoveTo(posx, posy); }
        void MoveTo(float posx, float posy) { _MoveTo(posx, posy); }
        void MoveTo(const Point &poi) { _MoveTo(poi); }

        virtual void SetSize(int aWidth, int aHeight) = 0;

    private:
        virtual int _GetX() const = 0;
        virtual int _GetY() const = 0;

        virtual int _GetWidth() const = 0;
        virtual int _GetHeight() const = 0;

        virtual float _GetZoom() const = 0;

        virtual void _SetZoom(float zoom) = 0;

        virtual void _MoveUp(int posy) = 0;
        virtual void _MoveDown(int posy) = 0;
        virtual void _MoveLeft(int posx) = 0;
        virtual void _MoveRight(int posx) = 0;
        virtual void _MoveTo(int posx, int posy) = 0;
        virtual void _MoveTo(float posx, float posy) = 0;
        virtual void _MoveTo(const Point &poi) = 0;
};
}
