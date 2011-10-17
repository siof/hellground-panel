/*
*    HG Players Panel - web panel for HellGround server Players
*    Copyright (C) 2011 HellGround Team : Siof, lukaasm,
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU Affero General Public License as
*    published by the Free Software Foundation, either version 3 of the
*    License, or (at your option) any later version.
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
#include <WStackedWidget>
#include <WAnimation>

#include "defines.h"
#include "pages/error.h"

using namespace Wt;

class HGMenuOption
{
public:
    HGMenuOption(MenuOptions menuOption, WObject * parent = NULL);
    ~HGMenuOption();

    void AddMenuItem(AccountLevel accLvl, uint32 textId, WMenuItem * menuItem);
    void AddMenuItem(AccountLevel accLvl, SessionInfo * sess, uint32 textId, WContainerWidget * item);
    void RemoveMenuItem(WMenuItem * menuItem, bool alsoDelete = true);
    void RemoveMenuItem(AccountLevel accLvl, bool alsoDelete = true);

    WMenuItem * GetMenuItemForLevel(AccountLevel accLvl);
    void UpdateTexts(SessionInfo * sess);
private:
    WObject * itemsParent;
    uint32 * textIds;
    MenuOptions menuOption;
    WMenuItem ** items;
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

    WAnimation anim;

    void LogMeIn();
    void SetPlLang();
    void SetEngLang();
    void RefreshMenuWidgets();
    void RefreshActiveMenuWidget();
    void ShowMenuOptions(bool addLogin = false);
    void UpdateMenuOptions();
    void ClearLogin();
    void ClearPass();
};

#endif // MENU_H_INCLUDED
