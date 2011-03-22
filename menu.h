#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include <WMenu>
#include <WMenuItem>
#include <WStackedWidget>

#include "defines.h"

using namespace Wt;

class HGMenu : public WContainerWidget
{
public:
    HGMenu(WStackedWidget * menuContents, SessionInfo * sess, WContainerWidget *parent=0);
    ~HGMenu();

    void refresh();         // overload, some link should be available only if user is logged in
private:
    WContainerWidget * container;       // contains menu + additional items added on menu side
    WStackedWidget * menuContents;      // container where menu items will be shown after click
    WMenu * menu;
    WLineEdit * login;
    WLineEdit * pass;
    WPushButton * btn;
    SessionInfo * session;
    WBreak ** breakTab;

    WPushButton * plLang;
    WPushButton * enLang;

    void LogMeIn();
    void SetPlLang();
    void SetEngLang();
    void RefreshMenuWidgets();
    void ShowMenuOptions();
};

#endif // MENU_H_INCLUDED
