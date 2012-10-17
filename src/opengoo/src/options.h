#ifndef OPTIONS_H
#define OPTIONS_H

#include <QWidget>

/*!
 * The options class is the option menu launchable from in-game menu or from mainmenu.
 * The in-game menu will be the same menu but with less active options.
 */

class options : public QWidget
{
    Q_OBJECT
public:
    explicit options(QWidget *parent = 0);
    
signals:
    
public slots:
    
};

#endif // OPTIONS_H
