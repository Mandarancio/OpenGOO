#pragma once

#include "og_camera.h"
#include "og_poi.h"
#include "OGLib/rect.h"
#include "OGLib/vector2d.h"

#include "wog_scene.h"
#include "wog_level.h"

#include <QRect>
#include <QPoint>

#include <memory>

using namespace std;

class OGWindowCamera
{
        static OGWindowCamera* pInstance_;

        unique_ptr<OGCamera> camera_;
        OGPoi* target_;
        Rect scene_;
        PoiList poiList_;

        float traveltime_;
        float pause_;
        float zoom_;

        Vector2D initialPosition_;
        Vector2D finalPosition_;
        Vector2D velocity_;

        bool isScrolling_;

        void _SetTarget();                

    public:
        OGWindowCamera(const Rect &scene, const Size& size, const WOGCamera* cam);
        ~OGWindowCamera();

        static OGWindowCamera* instance();

        float zoom() const { return camera_->zoom(); }

        const QPoint windowToLogical(const QPoint &p);
        QRect rect() const;

        void SetLastPosition();

        void ScrollUp(int shift);
        void ScrollDown(int shift);
        void ScrollLeft(int shift);
        void ScrollRight(int shift);

        void Update(int time);
};
