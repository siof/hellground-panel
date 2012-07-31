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

#include "menu.h"

#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WLengthValidator>
#include <Wt/WMenu>
#include <Wt/WPushButton>
#include <Wt/WRegExpValidator>
#include <Wt/WStackedWidget>
#include <Wt/WSubMenuItem>
#include <Wt/WTemplate>

#include "pages/default.h"
#include "pages/accInfo.h"
#include "pages/register.h"
#include "pages/passRecovery.h"
#include "pages/passChange.h"
#include "pages/serverStatus.h"
#include "pages/logout.h"
#include "pages/teleport.h"
#include "pages/licence.h"
#include "pages/support.h"
#include "pages/vote.h"
#include "pages/characters.h"

#include "database.h"
#include "misc.h"

HGMenu::HGMenu(WStackedWidget * menuContents, SessionInfo * sess, Wt::WTemplate * tmpl, Wt::Orientation ori, WContainerWidget *parent)
: WMenu(menuContents, ori, parent)
{
    session = sess;
    templ = tmpl;

    setRenderAsList(true);

    Wt::WMenu * tmpMenu;
    Wt::WMenuItem * tmpItem;
    Wt::WSubMenuItem * tmpSubMenu;

    AddMenuItem(TXT_MENU_HOME, new DefaultPage(), LVL_PLAYER, true);
    AddMenuItem(TXT_MENU_REGISTER, new RegisterPage(sess), LVL_NONE, true, "register");
    AddMenuItem(TXT_MENU_ACC_INFO, new AccountInfoPage(sess), LVL_PLAYER, false, "account");
    AddMenuItem(TXT_MENU_PASS_RECOVERY, new PassRecoveryPage(sess), LVL_NONE, true, "recover");
    AddMenuItem(TXT_MENU_PASS_CHANGE, new PassChangePage(sess), LVL_PLAYER, false, "changepassword");
    AddMenuItem(TXT_MENU_CHARACTERS, new CharacterInfoPage(sess), LVL_PLAYER, false, "characters");
    AddMenuItem(TXT_MENU_TELEPORT, new TeleportPage(sess), LVL_PLAYER, false, "teleport");

    tmpSubMenu = new Wt::WSubMenuItem(Wt::WString::tr(TXT_MENU_SUPPORT), new SupportPage(session));
    tmpSubMenu->setPathComponent("support");

    tmpMenu = new Wt::WMenu(menuContents, Wt::Vertical);
    tmpMenu->setInternalPathEnabled("/support");

    tmpItem = new Wt::WMenuItem(Wt::WString::tr(TXT_MENU_VOTE), new VotePage(session));
    tmpItem->setPathComponent("vote");
    tmpMenu->setRenderAsList(true);
    tmpMenu->addItem(tmpItem);
    tmpSubMenu->setSubMenu(tmpMenu);
    tmpMenu->select(-1);

    tmpMenu = NULL;
    menuItems.push_back(new MenuItemInfo(tmpSubMenu, LVL_PLAYER, false));
    tmpSubMenu = NULL;

    AddMenuItem(TXT_MENU_SERVER_STATUS, new ServerStatusPage(), LVL_PLAYER, true, "status");
    AddMenuItem(TXT_MENU_LOGOUT, new LogoutPage(sess, templ), LVL_PLAYER, false, "logout");
    AddMenuItem(TXT_MENU_LICENCE, new LicencePage(), LVL_PLAYER, true, "licence");

    for (std::vector<MenuItemInfo*>::const_iterator itr = menuItems.begin(); itr != menuItems.end(); ++itr)
        addItem((*itr)->item);

    UpdateMenuOptions();

    itemSelected().connect(this, &HGMenu::RefreshActiveMenuWidget);

    // must be here to provide internal path functionality
    setInternalPathEnabled("/");
}

void HGMenu::RefreshActiveMenuWidget()
{
    contentsStack()->currentWidget()->refresh();
}

void HGMenu::AddMenuItem(const char * txt, Wt::WWidget * contents, AccountLevel reqLvl, bool notLogged, const char * path)
{
    Wt::WMenuItem * tmpItem = new Wt::WMenuItem(Wt::WString::tr(txt), contents);

    if (path)
        tmpItem->setPathComponent(path);

    menuItems.push_back(new MenuItemInfo(tmpItem, reqLvl, notLogged));
}

HGMenu::~HGMenu()
{
    session = NULL;
}

void HGMenu::UpdateMenuOptions()
{
    for (std::vector<MenuItemInfo*>::iterator itr = menuItems.begin(); itr != menuItems.end(); ++itr)
    {
        MenuItemInfo * tmpItem = (*itr);

        // checks for menu permissions will be improved after moving account levels to permission masks
        if (session->accLvl >= tmpItem->reqAccLvl || (session->accLvl == LVL_NOT_LOGGED && tmpItem->notLoggedAlso))
            tmpItem->item->show();
        else
            tmpItem->item->hide();
    }
}

void HGMenu::refresh()
{
    Misc::Console(DEBUG_CODE, "\nHGMenu::refresh()\n");

    if (session->accLvl == LVL_LOGGED_OUT)
    {
        contentsStack()->refresh();
        session->accLvl = LVL_NOT_LOGGED;
    }

    UpdateMenuOptions();

    WMenu::refresh();
}
