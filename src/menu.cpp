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

#include "menu.h"

#include <Wt/WRegExpValidator>
#include <Wt/WLengthValidator>
#include <Wt/WSubMenuItem>

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

HGSubMenu::HGSubMenu(WStackedWidget * target, HGMenu * hgMenu, WSubMenuItem * parent)
{
    menu = new WMenu(target, Vertical);
    menu->setRenderAsList(true);
    menu->itemSelected().connect(hgMenu, &HGMenu::RefreshActiveMenuWidget);
    if (parent)
        parent->setSubMenu(menu);
}

HGSubMenu::~HGSubMenu()
{
    menu = NULL;
}

void HGSubMenu::AddMenuItem(const char * textId, WMenuItem * menuItem)
{
    if (!textId || !menuItem)
        return;

    items.push_back(menuItem);
    textIds.push_back(textId);
    menu->addItem(menuItem);
    menuItem = NULL;
}

void HGSubMenu::AddMenuItem(SessionInfo * sess, const char * textId, WContainerWidget * item, bool preload)
{
    if (!textId || !item)
        return;

    items.push_back(new WMenuItem(tr(textId), item, preload ? WMenuItem::PreLoading : WMenuItem::LazyLoading));
    textIds.push_back(textId);
    menu->addItem(items.back());
    item = NULL;
}

const std::list<WMenuItem*> HGSubMenu::GetMenuItems()
{
    return items;
}

WMenu * HGSubMenu::GetMenu()
{
    return menu;
}

void HGSubMenu::UpdateTexts()
{
    WMenuItem * tmpItem;
    std::list<const char *>::const_iterator textItr = textIds.begin();

    for (std::list<WMenuItem*>::const_iterator itr = items.begin(); itr != items.end() && textItr != textIds.end();
         ++itr, ++textItr)
    {
        if (tmpItem = *itr)
            tmpItem->setText(tr(*textItr));
    }
}

void HGSubMenu::Refresh()
{
    for (std::list<WMenuItem*>::iterator itr = items.begin(); itr != items.end(); ++itr)
        if (*itr)
            (*itr)->contents()->refresh();
}

HGMenuOption::HGMenuOption(MenuOptions menuOption):
    menuOption(menuOption)
{
    items = new WMenuItem * [ACCOUNT_LEVEL_COUNT];
    textIds = new const char *[ACCOUNT_LEVEL_COUNT];
    subMenus = new HGSubMenu * [ACCOUNT_LEVEL_COUNT];

    for (int i = 0; i < ACCOUNT_LEVEL_COUNT; ++i)
    {
        items[i] = NULL;
        textIds[i] = 0;
        subMenus[i] = NULL;
    }
}

HGMenuOption::~HGMenuOption()
{
    delete [] textIds;

    if (!items)
        return;

    for (int i = 0; i < ACCOUNT_LEVEL_COUNT; ++i)
    {
        if (items[i])
        {
            WWidget * tmpW = items[i]->itemWidget();

            if (tmpW)
                delete tmpW;

            tmpW = NULL;

            delete items[i];
            items[i] = NULL;
        }
    }

    delete [] items;
}

void HGMenuOption::AddMenuItem(AccountLevel accLvl, const char * textId, WMenuItem * menuItem)
{
    if (accLvl >= ACCOUNT_LEVEL_COUNT || accLvl < LVL_NOT_LOGGED || !menuItem)
        return;

    RemoveMenuItem(accLvl);

    items[accLvl+1] = menuItem;
    textIds[accLvl+1] = textId;
    menuItem = NULL;
}

void HGMenuOption::AddMenuItem(AccountLevel accLvl, SessionInfo * sess, const char * textId, WContainerWidget * item, const char * path, bool preload)
{
    if (accLvl >= ACCOUNT_LEVEL_COUNT || accLvl < LVL_NOT_LOGGED || !sess || !item)
        return;

    RemoveMenuItem(accLvl);

    items[accLvl+1] = new WMenuItem(tr(textId), item, preload ? WMenuItem::PreLoading : WMenuItem::LazyLoading);
    textIds[accLvl+1] = textId;
    if (path)
        items[accLvl+1]->setPathComponent(path);
    item = NULL;
}

void HGMenuOption::RemoveMenuItem(WMenuItem * menuItem, bool alsoDelete)
{
    if (!menuItem)
        return;

    for (int i = 0; i < ACCOUNT_LEVEL_COUNT; ++i)
    {
        if (items[i] == menuItem)
        {
            items[i] = NULL;
            textIds[i] = 0;
        }
    }

    if (alsoDelete)
    {
        delete menuItem->itemWidget();
        delete menuItem;
        menuItem = NULL;
    }
}

void HGMenuOption::RemoveMenuItem(AccountLevel accLvl, bool alsoDelete)
{
    if (accLvl >= ACCOUNT_LEVEL_COUNT || accLvl < LVL_NOT_LOGGED)
        return;

    if (alsoDelete)
    {
        WMenuItem * tmpI = items[accLvl+1];

        if (tmpI)
        {
            WObject * tmpObj = tmpI->itemWidget();
            delete tmpObj;
            tmpObj = NULL;
        }

        delete items[accLvl+1];
    }

    items[accLvl+1] = NULL;
    textIds[accLvl+1] = 0;
}

void HGMenuOption::AddSubMenuItem(AccountLevel accLvl, const char * textId, WSubMenuItem * menuItem, WStackedWidget * target, HGMenu * menu)
{
    if (accLvl >= ACCOUNT_LEVEL_COUNT || accLvl < LVL_NOT_LOGGED || !menuItem)
        return;

    RemoveSubMenuItem(accLvl);

    items[accLvl+1] = menuItem;
    textIds[accLvl+1] = textId;
    subMenus[accLvl+1] = new HGSubMenu(target, menu, menuItem);
    menuItem = NULL;
}

void HGMenuOption::AddSubMenuItem(AccountLevel accLvl, SessionInfo * sess, const char * textId, WContainerWidget * item, WStackedWidget * target, HGMenu * menu, const char * path, bool preload)
{
    if (accLvl >= ACCOUNT_LEVEL_COUNT || accLvl < LVL_NOT_LOGGED || !sess || !item)
        return;

    RemoveSubMenuItem(accLvl);

    items[accLvl+1] = new WSubMenuItem(tr(textId), item, preload ? WMenuItem::PreLoading : WMenuItem::LazyLoading);
    textIds[accLvl+1] = textId;
    subMenus[accLvl+1] = new HGSubMenu(target, menu, (WSubMenuItem*)items[accLvl+1]);
    item = NULL;

    if (path)
        items[accLvl+1]->setPathComponent(path);

    subMenus[accLvl+1]->GetMenu()->setInternalPathEnabled(items[accLvl+1]->pathComponent());
}

void HGMenuOption::RemoveSubMenuItem(AccountLevel accLvl, bool alsoDelete)
{
    if (accLvl >= ACCOUNT_LEVEL_COUNT || accLvl < LVL_NOT_LOGGED)
        return;

    if (alsoDelete)
    {
        WSubMenuItem * tmpI = (WSubMenuItem*)items[accLvl+1];

        if (tmpI)
        {
            WMenu * tmpMenu = tmpI->subMenu();
            delete tmpMenu;
            tmpMenu = NULL;
        }

        delete items[accLvl+1];
    }

    items[accLvl+1] = NULL;
    textIds[accLvl+1] = 0;
}

void HGMenuOption::AddSubMenuOption(AccountLevel accLvl, const char * textId, WMenuItem * menuItem)
{
    if (accLvl >= ACCOUNT_LEVEL_COUNT || accLvl < LVL_NOT_LOGGED || !menuItem)
        return;

    HGSubMenu * tmpSubMenu = subMenus[accLvl+1];

    if (tmpSubMenu)
        tmpSubMenu->AddMenuItem(textId, menuItem);
}

void HGMenuOption::AddSubMenuOption(AccountLevel accLvl, SessionInfo * sess, const char * textId, WContainerWidget * item, bool preload)
{
    if (accLvl >= ACCOUNT_LEVEL_COUNT || accLvl < LVL_NOT_LOGGED || !item)
        return;

    HGSubMenu * tmpSubMenu = subMenus[accLvl+1];

    if (tmpSubMenu)
        tmpSubMenu->AddMenuItem(sess, textId, item, preload);
}

WMenuItem * HGMenuOption::GetMenuItemForLevel(AccountLevel accLvl)
{
    if (accLvl >= ACCOUNT_LEVEL_COUNT || accLvl < LVL_NOT_LOGGED || accLvl < LVL_PLAYER)
        return items[LVL_NOT_LOGGED+1];

    // if menu item for given lvl doesn't exist, check lower levels
    for (int i = accLvl; i > LVL_NOT_LOGGED; --i)
        if (items[i+1])
            return items[i+1];

    return NULL;
}

void HGMenuOption::UpdateTexts()
{
    if (!items)
        return;

    WMenuItem * tmpItem;
    HGSubMenu * tmpSubMenu;

    for (int i = 0; i < ACCOUNT_LEVEL_COUNT; ++i)
    {
        if (tmpItem = items[i])
            tmpItem->setText(tr(textIds[i]));

        if (tmpSubMenu = subMenus[i])
            tmpSubMenu->UpdateTexts();
    }
}

void HGMenuOption::Refresh()
{
    WMenuItem * tmpItem;
    HGSubMenu * tmpSubMenu;
    for (int i = 0; i < ACCOUNT_LEVEL_COUNT; ++i)
    {
        if (tmpItem = items[i])
            tmpItem->contents()->refresh();

        if (tmpSubMenu = subMenus[i])
            tmpSubMenu->Refresh();
    }
}

HGMenu::HGMenu(WStackedWidget * menuContents, SessionInfo * sess, WContainerWidget *parent)
: WContainerWidget(parent)
{
    session = sess;
    this->menuContents = menuContents;
    container = this;

    WContainerWidget * tmpCont = new WContainerWidget();
    tmpCont->setContentAlignment(AlignCenter);
    plLang = new WPushButton("PL");
    plLang->clicked().connect(this, &HGMenu::SetPlLang);
    enLang = new WPushButton("EN");
    enLang->clicked().connect(this, &HGMenu::SetEngLang);

    tmpCont->addWidget(plLang);
    tmpCont->addWidget(enLang);

    addWidget(tmpCont);
    addWidget(new WBreak());
    addWidget(new WBreak());

    tmpCont = NULL;

    loginContainer = new WContainerWidget(this);

    login = new WLineEdit();
    login->setText(tr(TXT_ACC_LOGIN));
    login->setEchoMode(WLineEdit::Normal);
    login->focussed().connect(this, &HGMenu::ClearWLineEdit);
    WRegExpValidator * validator = new WRegExpValidator(LOGIN_VALIDATOR);
    validator->setMandatory(true);
    login->setValidator(validator);

    pass = new WLineEdit();
    pass->setText(WString("pass"));
    pass->setEchoMode(WLineEdit::Password);
    pass->focussed().connect(this, &HGMenu::ClearWLineEdit);
    WLengthValidator * lenValidator = new WLengthValidator(PASSWORD_LENGTH_MIN, PASSWORD_LENGTH_MAX);
    lenValidator->setMandatory(true);
    pass->setValidator(lenValidator);

    btnLog = new WPushButton(tr(TXT_BTN_LOGIN));
    btnLog->clicked().connect(this, &HGMenu::LogMeIn);

    breakTab = new WBreak*[3];
    for (int i = 0; i < 3; i++)
        breakTab[i] = new WBreak();

    loginContainer->addWidget(login);
    loginContainer->addWidget(breakTab[0]);
    loginContainer->addWidget(pass);
    loginContainer->addWidget(breakTab[1]);
    loginContainer->addWidget(btnLog);
    loginContainer->addWidget(breakTab[2]);

    menu = new WMenu(menuContents, Wt::Vertical, this);
    menu->setRenderAsList(true);

    for (int i = 0; i < MENU_SLOT_COUNT; ++i)
        menuSlots[i] = NULL;

    menuSlots[MENU_SLOT_HOME] = new HGMenuOption(MENU_SLOT_HOME);
    menuSlots[MENU_SLOT_HOME]->AddMenuItem(LVL_NOT_LOGGED, session, TXT_MENU_HOME, new DefaultPage(sess), "");
    menuSlots[MENU_SLOT_HOME]->AddMenuItem(LVL_PLAYER, TXT_MENU_HOME, menuSlots[MENU_SLOT_HOME]->GetMenuItemForLevel(LVL_NOT_LOGGED));

    menuSlots[MENU_SLOT_ACCOUNT] = new HGMenuOption(MENU_SLOT_ACCOUNT);
    menuSlots[MENU_SLOT_ACCOUNT]->AddMenuItem(LVL_NOT_LOGGED, session, TXT_MENU_REGISTER, new RegisterPage(sess), "account");
    menuSlots[MENU_SLOT_ACCOUNT]->AddMenuItem(LVL_PLAYER, session, TXT_MENU_ACC_INFO, new AccountInfoPage(sess), "account", false);

    menuSlots[MENU_SLOT_PASSWORD] = new HGMenuOption(MENU_SLOT_PASSWORD);
    menuSlots[MENU_SLOT_PASSWORD]->AddMenuItem(LVL_NOT_LOGGED, session, TXT_MENU_PASS_RECOVERY, new PassRecoveryPage(sess), "password");
    menuSlots[MENU_SLOT_PASSWORD]->AddMenuItem(LVL_PLAYER, session, TXT_MENU_PASS_CHANGE, new PassChangePage(sess), "passchange");

    menuSlots[MENU_SLOT_CHARACTERS] = new HGMenuOption(MENU_SLOT_CHARACTERS);
    menuSlots[MENU_SLOT_CHARACTERS]->AddMenuItem(LVL_PLAYER, session, TXT_MENU_CHARACTERS, new CharacterInfoPage(sess), "characters");

    menuSlots[MENU_SLOT_TELEPORT] = new HGMenuOption(MENU_SLOT_TELEPORT);
    menuSlots[MENU_SLOT_TELEPORT]->AddMenuItem(LVL_PLAYER, session, TXT_MENU_TELEPORT, new TeleportPage(session), "teleport");

    menuSlots[MENU_SLOT_SUPPORT] = new HGMenuOption(MENU_SLOT_SUPPORT);
    menuSlots[MENU_SLOT_SUPPORT]->AddSubMenuItem(LVL_PLAYER, session, TXT_MENU_SUPPORT, new SupportPage(session), menuContents, this, "support");
    menuSlots[MENU_SLOT_SUPPORT]->AddSubMenuOption(LVL_PLAYER, session, TXT_MENU_VOTE, new VotePage(session));

    menuSlots[MENU_SLOT_SERVER_STATUS] = new HGMenuOption(MENU_SLOT_SERVER_STATUS);
    menuSlots[MENU_SLOT_SERVER_STATUS]->AddMenuItem(LVL_NOT_LOGGED, session, TXT_MENU_SERVER_STATUS, new ServerStatusPage(), "status");
    menuSlots[MENU_SLOT_SERVER_STATUS]->AddMenuItem(LVL_PLAYER, TXT_MENU_SERVER_STATUS, menuSlots[MENU_SLOT_SERVER_STATUS]->GetMenuItemForLevel(LVL_NOT_LOGGED));

    menuSlots[MENU_SLOT_LOGIN] = new HGMenuOption(MENU_SLOT_LOGIN);
    menuSlots[MENU_SLOT_LOGIN]->AddMenuItem(LVL_PLAYER, session, TXT_MENU_LOGOUT, new LogoutPage(session), "log");

    menuSlots[MENU_SLOT_LICENCE] = new HGMenuOption(MENU_SLOT_LICENCE);
    menuSlots[MENU_SLOT_LICENCE]->AddMenuItem(LVL_NOT_LOGGED, session, TXT_MENU_LICENCE, new LicencePage(), "licence");
    menuSlots[MENU_SLOT_LICENCE]->AddMenuItem(LVL_PLAYER, TXT_MENU_LICENCE, menuSlots[MENU_SLOT_LICENCE]->GetMenuItemForLevel(LVL_NOT_LOGGED));

    menuSlots[MENU_SLOT_ERROR] = new HGMenuOption(MENU_SLOT_ERROR);
    menuSlots[MENU_SLOT_ERROR]->AddMenuItem(LVL_NOT_LOGGED, session, TXT_MENU_ERROR, new ErrorPage(session));
    //menuSlots[MENU_SLOT_ERROR]->AddMenuItem(LVL_NOT_LOGGED, TXT_MENU_ERROR, new WMenuItem(tr(TXT_MENU_ERROR), new ErrorPage(session)));
    menuSlots[MENU_SLOT_ERROR]->AddMenuItem(LVL_PLAYER, TXT_MENU_ERROR, menuSlots[MENU_SLOT_ERROR]->GetMenuItemForLevel(LVL_NOT_LOGGED));
    menuSlots[MENU_SLOT_ERROR]->GetMenuItemForLevel(LVL_PLAYER)->hide();
    menuSlots[MENU_SLOT_ERROR]->GetMenuItemForLevel(LVL_PLAYER)->disable();

    ShowMenuOptions();

    menu->itemSelected().connect(this, &HGMenu::RefreshActiveMenuWidget);
    addWidget(menu);

    // must be here to provide internal path functionality
    menu->setInternalPathEnabled("/");
}

HGMenu::~HGMenu()
{
    delete menu;
    delete login;
    delete pass;
    delete btnLog;

    for (int i = 0; i < 3; i++)
        delete breakTab[i];

    delete [] breakTab;

    clear();
}

void HGMenu::RefreshMenuWidgets()
{
    int count = menuContents->count();
    for (int i = 0; i < count; ++i)
        menuContents->widget(i)->refresh();
}

void HGMenu::RefreshActiveMenuWidget()
{
    menuContents->currentWidget()->refresh();
}

void HGMenu::LogMeIn()
{
    bool validLogin = login->validate() == WValidator::Valid;
    bool validPass = pass->validate() == WValidator::Valid;

    if (!validLogin || !validPass)
    {
        Log(LOG_INVALID_DATA, "User trying to log in with invalid data ! ip: %s login: %s pass: %s", session->sessionIp.toUTF8().c_str(), login->text().toUTF8().c_str(), pass->text().toUTF8().c_str());
        ShowError(ERROR_SLOT_ADDITIONAL, TXT_ERROR_VALIDATION_LOGIN);
        return;
    }

    Database db;
    if (!db.Connect(SERVER_DB_DATA, SQL_REALMDB))
    {
        SetError(ERROR_SLOT_BASE, TXT_MENU_ERROR);
        std::string tmpErr = db.GetError();
        SetError(ERROR_SLOT_DB, tmpErr);
        ShowError();
        ClearLoginData();
        return;
    }

    // simple login check ... should be replaced with something better

    std::string escapedLogin = db.EscapeString(login->text());
    std::string escapedPass = db.EscapeString(pass->text());
    WString shapass;

    switch (db.ExecutePQuery("SELECT SHA1(UPPER('%s:%s'))", escapedLogin.c_str(), escapedPass.c_str()))
    {
        case DB_RESULT_ERROR:
        {
            AddActivityLogIn(false, escapedLogin.c_str());
            // if there was database error
            std::string tmpErr = db.GetError();
            ShowError(ERROR_SLOT_DB, tmpErr);
            ClearLoginData();
            return;
        }
        case DB_RESULT_EMPTY:
        {
            AddActivityLogIn(false, escapedLogin.c_str());
            Log(LOG_STRANGE_DATA, "User with IP: %s tried to login with strange data (SHA return empty)! login: %s pass: %s", session->sessionIp.toUTF8().c_str(), escapedLogin.c_str(), escapedPass.c_str());
            //if wrong data
            ShowError(ERROR_SLOT_ADDITIONAL, TXT_ERROR_WRONG_LOGIN_DATA);
            ClearLoginData();
            return;
        }
        default:
        {
            shapass = db.GetRow()->fields[0].GetWString();
            break;
        }
    }

               //           0            1         2     3       4       5         6       7         8       9         10
    db.SetPQuery("SELECT username, sha_pass_hash, id, gmlevel, email, joindate, last_ip, locked, expansion, vote, account_flags "
                 "FROM account "
                 "WHERE username = '%s'", escapedLogin.c_str());

    // execute will return 0 if result will be empty and -1 if there will be DB error.
    switch (db.ExecuteQuery())
    {
        case DB_RESULT_ERROR:
        {
            AddActivityLogIn(false, escapedLogin.c_str());
            // if there was database error
            std::string tmpErr = db.GetError();
            ShowError(ERROR_SLOT_DB, tmpErr);
            ClearLoginData();
            return;
        }
        case DB_RESULT_EMPTY:
        {
            AddActivityLogIn(false, escapedLogin.c_str());
            //if wrong data
            ShowError(ERROR_SLOT_ADDITIONAL, TXT_ERROR_WRONG_LOGIN_DATA);
            ClearLoginData();
            return;
        }
        default:
        {
            DatabaseRow * row = db.GetRow();

            if (!row)
            {
                std::string tmpErr = "ERROR: Row not found!";
                ShowError(ERROR_SLOT_BASE, tmpErr);
                ClearLoginData();
                return;
            }

            if (row->fields[1].GetWString() != shapass)
            {
                AddActivityLogIn(row->fields[2].GetUInt32(), false);
                //if wrong data
                ShowError(ERROR_SLOT_ADDITIONAL, TXT_ERROR_WRONG_LOGIN_DATA);
                ClearLoginData();
                return;
            }

            if (row->fields[7].GetBool() && row->fields[6].GetWString() != session->sessionIp)
            {
                AddActivityLogIn(row->fields[2].GetUInt32(), false);
                ShowError(ERROR_SLOT_ADDITIONAL, TXT_ERROR_IP_MISMATCH);
                ClearLoginData();
                return;
            }

            AddActivityLogIn(row->fields[2].GetUInt32(), true);

            session->login = row->fields[0].GetWString();
            session->pass = row->fields[1].GetWString();
            session->accid = row->fields[2].GetUInt64();
            session->accLvl = row->fields[3].GetAccountLevel();
            session->email = row->fields[4].GetWString();
            session->joinDate = row->fields[5].GetWString();
            session->lastIp = row->fields[6].GetWString();
            session->locked = row->fields[7].GetBool();
            session->expansion = row->fields[8].GetInt();
            session->vote = row->fields[9].GetUInt32();
            session->account_flags = row->fields[10].GetUInt64();

            if (db.ExecutePQuery("SELECT * FROM account_banned WHERE id = '%u' AND active = 1 AND (bandate = unbandate OR unbandate > UNIX_TIMESTAMP())", session->accid) > DB_RESULT_EMPTY)
                session->banned = true;

            login->setText("");
            pass->setText("");

//            WAnimation fade(WAnimation::Fade, WAnimation::Linear, 250);
//            login->setHidden(true, fade);
//            pass->setHidden(true, fade);
//            btnLog->setHidden(true, fade);
//            login->setDisabled(true);
//            pass->setDisabled(true);
//            btnLog->setDisabled(true);
            loginContainer->setHidden(true);

            if (db.Connect(PANEL_DB_DATA, SQL_PANELDB))
            {
                db.ExecutePQuery("DELETE FROM AccVote WHERE acc = '%u' AND resetDate < NOW()", session->accid);
                db.ExecutePQuery("DELETE FROM IPVote WHERE ip = '%s' AND resetDate < NOW()", session->sessionIp.toUTF8().c_str());
            }

            ShowMenuOptions();
            refresh();

            WMenuItem * tmpItem = menuSlots[MENU_SLOT_ERROR]->GetMenuItemForLevel(LVL_PLAYER);
            if (tmpItem)
                tmpItem->hide();

            menu->select(0);
            RefreshActiveMenuWidget();

            break;
        }
    }

    ClearLoginData();
}

void HGMenu::SetPlLang()
{
    wApp->setLocale("pl");
}

void HGMenu::SetEngLang()
{
    wApp->setLocale("en");
}

void HGMenu::ShowMenuOptions(bool addLogin)
{
    if (!menuSlots)
        return;

    WMenuItem * tmp;

    std::vector<WMenuItem*>::const_iterator itr = menu->items().begin();
    while (itr != menu->items().end())
    {
        tmp = *itr;
        //++itr;
        menu->removeItem(tmp);
        tmp = NULL;
        itr = menu->items().begin();
    }

    HGMenuOption * tmpOption = NULL;

    for (int i = 0; i < MENU_SLOT_COUNT; ++i)
        if (tmpOption = menuSlots[i])
            if (tmp = tmpOption->GetMenuItemForLevel(session->accLvl))
                menu->addItem(tmp);

    tmp = NULL;
    tmpOption = NULL;

    // if player isn't logged
    if (session->accid == 0)
    {
        if (login)
            login->setText(tr(TXT_ACC_LOGIN));

        if (pass)
            pass->setText(WString("pass"));

        if (btnLog)
            btnLog->setText(tr(TXT_BTN_LOGIN));

        if (addLogin)
        {
            loginContainer->setHidden(false);
//            WAnimation fade(WAnimation::Pop, WAnimation::Linear, 250);
//            login->setHidden(false, fade);
//            pass->setHidden(false, fade);
//            btnLog->setHidden(false, fade);
//            login->setDisabled(false);
//            pass->setDisabled(false);
//            btnLog->setDisabled(false);
        }
    }
}

void HGMenu::UpdateMenuOptions()
{
    HGMenuOption * tmpOption = NULL;
    for (int i = 0; i < MENU_SLOT_COUNT; ++i)
        if (tmpOption = menuSlots[i])
            tmpOption->UpdateTexts();

    tmpOption = NULL;

    if (login)
        login->setText(tr(TXT_ACC_LOGIN));

    if (pass)
        pass->setText(WString("pass"));

    if (btnLog)
        btnLog->setText(tr(TXT_BTN_LOGIN));
}

void HGMenu::refresh()
{
    console(DEBUG_CODE, "\nHGMenu::refresh()\n");

    if (session->accLvl != LVL_LOGGED_OUT)
    {
        UpdateMenuOptions();
        RefreshActiveMenuWidget();
    }
    else
    {
        // reset pages that need this
        menuSlots[MENU_SLOT_ACCOUNT]->Refresh();
        menuSlots[MENU_SLOT_SUPPORT]->Refresh();
        menuSlots[MENU_SLOT_CHARACTERS]->Refresh();

        session->accLvl = LVL_NOT_LOGGED;
        menu->select(0);
        ShowMenuOptions(true);
    }

    WContainerWidget::refresh();
}

void HGMenu::ClearLoginData()
{
    if (login)
        login->setText("");

    if (pass)
        pass->setText("");
}

bool HGMenu::SetError(ErrorSlots error, std::string &msg, ErrorPage * err)
{
    console(DEBUG_CODE, "\nHGMenu::SetError(ErrorSlots error = %i, std::string &msg = %s, ErrorPage * err = %i) overload: 1\n", error, msg.c_str(), err ? 1 : 0);

    ErrorPage * tmpError = err;
    if (!tmpError)
    {
        WMenuItem * tmpItem = menuSlots[MENU_SLOT_ERROR]->GetMenuItemForLevel(session->accLvl);

        if (!tmpItem)
            return false;

        ErrorPage * tmpError = (ErrorPage*)tmpItem->contents();

        if (!tmpError)
            return false;
    }

    WString tmpErr = WString::fromUTF8(msg);
    tmpError->SetErrorMsg(error, tmpErr);
    return true;
}

bool HGMenu::SetError(ErrorSlots error, const char * textId, ErrorPage * err)
{
    console(DEBUG_CODE, "\nHGMenu::SetError(ErrorSlots error = %i, const char * textId = %s, ErrorPage * err = %i) overload: 2\n", error, textId, err ? 1 : 0);

    ErrorPage * tmpError = err;
    if (!tmpError)
    {
        WMenuItem * tmpItem = menuSlots[MENU_SLOT_ERROR]->GetMenuItemForLevel(session->accLvl);

        if (!tmpItem)
            return false;

        ErrorPage * tmpError = (ErrorPage*)tmpItem->contents();

        if (!tmpError)
            return false;
    }

    tmpError->SetErrorMsg(error, textId);

    return true;
}

void HGMenu::ShowError(ErrorSlots error, std::string &msg)
{
    console(DEBUG_CODE, "\nHGMenu::ShowError(ErrorSlots error = %i, std::string &msg = %s) overload: 1\n", error, msg.c_str());

    WMenuItem * tmpItem = menuSlots[MENU_SLOT_ERROR]->GetMenuItemForLevel(session->accLvl);
    if (!tmpItem)
        return;

    ErrorPage * tmpError = (ErrorPage*)tmpItem->contents();
    if (!tmpError)
        return;

    SetError(error, msg, tmpError);

    menu->select(tmpItem);
    RefreshActiveMenuWidget();
}

void HGMenu::ShowError(ErrorSlots error, const char * textId)
{
    console(DEBUG_CODE, "\nHGMenu::ShowError(ErrorSlots error = %i, const char * textId = %s) overload: 2\n", error, textId);

    WMenuItem * tmpItem = menuSlots[MENU_SLOT_ERROR]->GetMenuItemForLevel(session->accLvl);
    if (!tmpItem)
        return;

    ErrorPage * tmpError = (ErrorPage*)tmpItem->contents();
    if (!tmpError)
        return;

    SetError(error, textId, tmpError);

    menu->select(tmpItem);
    RefreshActiveMenuWidget();
}

void HGMenu::ShowError()
{
    console(DEBUG_CODE, "\nHGMenu::ShowError() overload: 3\n");

    WMenuItem * tmpItem = menuSlots[MENU_SLOT_ERROR]->GetMenuItemForLevel(session->accLvl);
    if (!tmpItem)
        return;

    menu->select(tmpItem);
    RefreshActiveMenuWidget();
}

void HGMenu::AddActivityLogIn(bool success, const char * login)
{
    Database db;

    uint32 accId = session->accid;

    if (!accId)
    {
        if (!login || !db.Connect(SERVER_DB_DATA, SQL_REALMDB))
            return;

        if (db.ExecutePQuery("SELECT id FROM account WHERE username = '%s'", login) > DB_RESULT_EMPTY)
            accId = db.GetRow()->fields[0].GetUInt32();
        else
            return;
    }

    AddActivityLogIn(accId, success);
}

void HGMenu::AddActivityLogIn(uint32 id, bool success)
{
    if (!id)
        return;

    Database db;

    db.Connect(PANEL_DB_DATA, SQL_PANELDB);
    db.ExecutePQuery("INSERT INTO Activity VALUES ('%u', NOW(), '%s', '%s', '')", id, session->sessionIp.toUTF8().c_str(), success ? TXT_ACT_LOGIN_SUCCESS : TXT_ACT_LOGIN_FAIL);
}
