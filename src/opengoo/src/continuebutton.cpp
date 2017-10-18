#include "continuebutton.h"
#include "uidata.h"
#include "og_utils.h"

using namespace og;

ContinueButton::ContinueButton()
{
    auto data = utils::getUIData("CONTINUE_BUTTON");
    setSize(data->width, data->height);
    setText("Continue");
    setUpImage(utils::getImage(data->up));
    setOverImage(utils::getImage(data->over));
}
