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

    AddMenuItem(TXT_MENU_HOME, new DefaultPage(), PERM_PLAYER, true);
    AddMenuItem(TXT_MENU_REGISTER, new RegisterPage(sess), PERM_NONE, true, "register");
    AddMenuItem(TXT_MENU_ACC_INFO, new AccountInfoPage(sess), PERM_NONE, false, "account");
    AddMenuItem(TXT_MENU_PASS_RECOVERY, new PassRecoveryPage(sess), PERM_NONE, true, "recover");
    AddMenuItem(TXT_MENU_PASS_CHANGE, new PassChangePage(sess), PERM_PLAYER, false, "changepassword");
    AddMenuItem(TXT_MENU_CHARACTERS, new CharacterInfoPage(sess), PERM_PLAYER, false, "characters");
    AddMenuItem(TXT_MENU_TELEPORT, new TeleportPage(sess), PERM_PLAYER, false, "teleport");
    AddMenuItem(TXT_MENU_SUPPORT, new SupportPage(sess), PERM_PLAYER, false, "support");
    AddMenuItem(TXT_MENU_SERVER_STATUS, new ServerStatusPage(), PERM_PLAYER, true, "status");
    AddMenuItem(TXT_MENU_LOGOUT, new LogoutPage(sess, templ), PERM_PLAYER, false, "logout");
    AddMenuItem(TXT_MENU_LICENCE, new LicencePage(), PERM_PLAYER, true, "licence");

    for (std::list<MenuItemInfo*>::const_iterator itr = menuItems.begin(); itr != menuItems.end(); ++itr)
        addItem((*itr)->item);

    UpdateMenuOptions();

    itemSelected().connect(this, &HGMenu::RefreshActiveMenuWidget);

    // must be here to provide internal path functionality
    setInternalPathEnabled("/");

    setStyleClass("menuwidget");
}

void HGMenu::RefreshActiveMenuWidget()
{
    contentsStack()->currentWidget()->refresh();
}

void HGMenu::AddMenuItem(const char * txt, Wt::WWidget * contents, uint64 reqPerms, bool notLogged, const char * path)
{
    Wt::WMenuItem * tmpItem = new Wt::WMenuItem(Wt::WString::tr(txt), contents);

    if (path)
        tmpItem->setPathComponent(path);

    menuItems.push_back(new MenuItemInfo(tmpItem, reqPerms, notLogged));
}

HGMenu::~HGMenu()
{
    session = NULL;
}

void HGMenu::UpdateMenuOptions()
{
    for (std::list<MenuItemInfo*>::iterator itr = menuItems.begin(); itr != menuItems.end(); ++itr)
    {
        MenuItemInfo * tmpItem = (*itr);

        // checks for menu permissions will be improved after moving account levels to permission masks
        if (session->permissions & tmpItem->reqPermissions || (session->sessionState == SESSION_STATE_NOT_LOGGED && tmpItem->notLoggedAlso))
            tmpItem->item->show();
        else
            tmpItem->item->hide();
    }
}

void HGMenu::refresh()
{
    Misc::Console(DEBUG_CODE, "\nHGMenu::refresh()\n");

    if (session->sessionState == SESSION_STATE_LOGGED_OUT)
    {
        contentsStack()->refresh();
        session->sessionState = SESSION_STATE_NOT_LOGGED;
    }

    UpdateMenuOptions();

    WMenu::refresh();
}
