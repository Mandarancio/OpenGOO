#include "continuebutton.h"
#include "og_utils.h"

using namespace ogUtils;

ContinueButton::ContinueButton()
{
    auto data = getUIData("CONTINUE_BUTTON");
    setSize(data->width, data->height);
    setText("Continue");
    setUpImage(getImage(data->up));
    setOverImage(getImage(data->over));
}
