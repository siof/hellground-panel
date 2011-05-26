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

#include "defines.h"
#include "pages/error.h"

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
    ErrorPage * errorPage;
    WMenuItem * errorPageMenuItem;
    WBreak ** breakTab;

    WPushButton * plLang;
    WPushButton * enLang;

    void LogMeIn();
    void SetPlLang();
    void SetEngLang();
    void RefreshMenuWidgets();
    void RefreshActiveMenuWidget();
    void ShowMenuOptions();
};

#endif // MENU_H_INCLUDED
