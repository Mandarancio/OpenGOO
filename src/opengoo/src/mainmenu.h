 #ifndef MAINMENU_H
#define MAINMENU_H
#include "level.h"
#include "backgroundwidget.h"

class MainMenu : public Level
{
    Q_OBJECT
public:
    explicit MainMenu(BackGroundWidget *, QWidget *parent = 0);
    
signals:
    
public slots:
    
};

#endif // MAINMENU_H
