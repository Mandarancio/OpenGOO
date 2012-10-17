#include "mainmenu.h"

#define MAINMENUFILE "test"

MainMenu::MainMenu(BackGroundWidget *bgWidget, QWidget *parent) :
    Level(MAINMENUFILE, bgWidget, parent)
{
    //grab keyboard, mouse and track it!
    this->grabKeyboard();
    this->grabMouse();
    this->setMouseTracking(true);
}
