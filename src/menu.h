/*
*    HG Players Panel - web panel for HellGround server Players
*    Copyright (C) 2011 HellGround Team : Siof, lukaasm,
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU Affero General Public License version 3 as
*    published by the Free Software Foundation.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Affero General Public License for more details.
*
*    You should have received a copy of the GNU Affero General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include <WMenu>
#include <WMenuItem>

#include "defines.h"
#include "pages/error.h"

using namespace Wt;

class HGMenu;

class HGSubMenu
{
public:
    HGSubMenu(WStackedWidget * target, HGMenu * hgMenu, WSubMenuItem * parent = NULL);
    ~HGSubMenu();

    void AddMenuItem(uint32 textId, WMenuItem * menuItem);
    void AddMenuItem(SessionInfo * sess, uint32 textId, WContainerWidget * item, bool preload = true);

    const std::list<WMenuItem*> GetMenuItems();
    WMenu * GetMenu();
    void UpdateTexts(SessionInfo * sess);

    void Clear();

private:
    std::list<uint32> textIds;
    std::list<WMenuItem*> items;
    WMenu * menu;
};

class HGMenuOption
{
public:
    HGMenuOption(MenuOptions menuOption);
    ~HGMenuOption();

    void AddMenuItem(AccountLevel accLvl, uint32 textId, WMenuItem * menuItem);
    void AddMenuItem(AccountLevel accLvl, SessionInfo * sess, uint32 textId, WContainerWidget * item, const char * path = NULL, bool preload = true);
    void RemoveMenuItem(WMenuItem * menuItem, bool alsoDelete = true);
    void RemoveMenuItem(AccountLevel accLvl, bool alsoDelete = true);

    void AddSubMenuItem(AccountLevel accLvl, uint32 textId, WSubMenuItem * menuItem, WStackedWidget * target, HGMenu * menu);
    void AddSubMenuItem(AccountLevel accLvl, SessionInfo * sess, uint32 textId, WContainerWidget * item, WStackedWidget * target, HGMenu * menu, const char * path = NULL, bool preload = true);
    void RemoveSubMenuItem(AccountLevel accLvl, bool alsoDelete = true);
    void AddSubMenuOption(AccountLevel accLvl, uint32 textId, WMenuItem * menuItem);
    void AddSubMenuOption(AccountLevel accLvl, SessionInfo * sess, uint32 textId, WContainerWidget * item, bool preload = true);

    WMenuItem * GetMenuItemForLevel(AccountLevel accLvl);
    void UpdateTexts(SessionInfo * sess);

private:
    uint32 * textIds;
    MenuOptions menuOption;
    WMenuItem ** items;
    HGSubMenu ** subMenus;
};

class HGMenu : public WContainerWidget
{
public:
    HGMenu(WStackedWidget * menuContents, SessionInfo * sess, WContainerWidget *parent=0);
    ~HGMenu();

    void refresh();         // overload, some link should be available only if user is logged in
    void ShowError(ErrorSlots error, std::string &msg);
    void ShowError(ErrorSlots error, uint32 textId = 0);
    void ShowError();
    bool SetError(ErrorSlots error, std::string &msg, ErrorPage * err = NULL);
    bool SetError(ErrorSlots error, uint32 textId = 0, ErrorPage * err = NULL);

    void RefreshActiveMenuWidget();

private:
    WContainerWidget * container;       // contains menu + additional items added on menu side
    WContainerWidget * loginContainer;  // contains login forms
    WStackedWidget * menuContents;      // container where menu items will be shown after click
    WMenu * menu;
    WLineEdit * login;
    WLineEdit * pass;
    WPushButton * btnLog;
    SessionInfo * session;
    WBreak ** breakTab;

    WPushButton * plLang;
    WPushButton * enLang;

    HGMenuOption * menuSlots[MENU_SLOT_COUNT];

    void LogMeIn();
    void SetPlLang();
    void SetEngLang();
    void RefreshMenuWidgets();
    void ShowMenuOptions(bool addLogin = false);
    void UpdateMenuOptions();
    void ClearLoginData();

    void AddActivityLogIn(bool success, const char * login = NULL);
    void AddActivityLogIn(uint32 id, bool success);

    void ClearWLineEdit()
    {
        if (WObject::sender())
            ((WLineEdit*)WObject::sender())->setText("");
    }
};

#endif // MENU_H_INCLUDED
