#include "og_camera.h"
#include "OGLib/util.h"

int OGCamera::_GetWidth() const { return RoundF(size_.width() * zoom_); }
int OGCamera::_GetHeight() const { return RoundF(size_.height() * zoom_); }

void OGCamera::_MoveTo(float posx, float posy)
{
    int x = RoundF(posx);
    int y = RoundF(posy);
    pos_ = Point(x, y);
}
