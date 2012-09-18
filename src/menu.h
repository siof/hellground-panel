/*
*    HG Players Panel - web panel for HellGround server Players
*    Copyright (C) 2011-2012 HellGround Team : Siof, lukaasm,
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

#include <vector>

#include <Wt/WMenu>
#include <Wt/WMenuItem>

#include "defines.h"

struct MenuItemInfo
{
    MenuItemInfo() : item(NULL), reqAccLvl(LVL_NOT_LOGGED), notLoggedAlso(false) {}
    MenuItemInfo(const Wt::WString &text, Wt::WWidget *contents, AccountLevel reqLvl, bool notLogged, Wt::WMenuItem::LoadPolicy policy = Wt::WMenuItem::LazyLoading)
    {
        item = new Wt::WMenuItem(text, contents, policy);
        reqAccLvl = reqLvl;
        notLoggedAlso = notLogged;
    }

    MenuItemInfo(Wt::WMenuItem * menuItem, AccountLevel reqLvl, bool notLogged)
    {
        item = menuItem;
        reqAccLvl = reqLvl;
        notLoggedAlso = notLogged;
    }

    ~MenuItemInfo()
    {
        item = NULL;
        reqAccLvl = LVL_NOT_LOGGED;
        notLoggedAlso = false;
    }


    Wt::WMenuItem * item;
    AccountLevel reqAccLvl;
    bool notLoggedAlso;
};

class HGMenu : public Wt::WMenu
{
public:
    HGMenu(Wt::WStackedWidget * menuContents, SessionInfo * sess, Wt::WTemplate * tmpl, Wt::Orientation ori = Wt::Vertical, Wt::WContainerWidget *parent=0);
    ~HGMenu();

    void refresh();         // overload, some link should be available only if user is logged in

private:
    SessionInfo * session;
    WTemplate * templ;

    std::list<MenuItemInfo*> menuItems;

    void AddMenuItem(const char * txt, Wt::WWidget * contents, AccountLevel reqLvl, bool notLoggedAlso, const char * path = "");

    void UpdateMenuOptions();
    void RefreshActiveMenuWidget();
};

#endif // MENU_H_INCLUDED
