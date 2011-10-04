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

#include "menu.h"
#include "pages/default.h"
#include "pages/accInfo.h"
#include "pages/register.h"
#include "database.h"

HGMenuOption::HGMenuOption(MenuOptions menuOption, WObject * parent):
    itemsParent(parent), menuOption(menuOption)
{
    items = new WMenuItem*[ACCOUNT_LEVEL_COUNT];
    textIds = new uint32[ACCOUNT_LEVEL_COUNT];

    for (int i = 0; i < ACCOUNT_LEVEL_COUNT; ++i)
    {
        items[i] = NULL;
        textIds[i] = 0;
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

void HGMenuOption::AddMenuItem(AccountLevel accLvl, uint32 textId, WMenuItem * menuItem)
{
    if (accLvl >= ACCOUNT_LEVEL_COUNT || !menuItem)
        return;

    RemoveMenuItem(accLvl);

    items[accLvl+1] = menuItem;
    textIds[accLvl+1] = textId;
    menuItem = NULL;
}

void HGMenuOption::AddMenuItem(AccountLevel accLvl, SessionInfo * sess, uint32 textId, WContainerWidget * item)
{
    if (accLvl >= ACCOUNT_LEVEL_COUNT || !sess || !item)
        return;

    RemoveMenuItem(accLvl);

    items[accLvl+1] = new WMenuItem(sess->GetText(textId), item);
    textIds[accLvl+1] = textId;
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
    if (accLvl >= ACCOUNT_LEVEL_COUNT)
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

WMenuItem * HGMenuOption::GetMenuItemForLevel(AccountLevel accLvl)
{
    if (accLvl >= ACCOUNT_LEVEL_COUNT || accLvl < LVL_PLAYER)
        return items[LVL_NOT_LOGGED+1];

    // if menu item for given lvl doesn't exist, check lower logged levels
    for (int i = accLvl; i > LVL_NOT_LOGGED; --i)
        if (items[i+1])
            return items[i+1];

    return NULL;
}

void HGMenuOption::UpdateTexts(SessionInfo * sess)
{
    if (!items || !sess)
        return;

    for (int i = 0; i < ACCOUNT_LEVEL_COUNT; ++i)
        if (WMenuItem * tmpItem = items[i])
            tmpItem->setText(sess->GetText(textIds[i]));
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

    login = new WLineEdit();
    login->setText(session->GetText(TXT_LBL_ACC_LOGIN));
    login->setEchoMode(WLineEdit::Normal);
    login->focussed().connect(this, &HGMenu::ClearLogin);

    pass = new WLineEdit();
    pass->setText(WString("pass"));
    pass->setEchoMode(WLineEdit::Password);
    pass->focussed().connect(this, &HGMenu::ClearPass);

    btnLog = new WPushButton(session->GetText(TXT_BTN_LOGIN));
    btnLog->clicked().connect(this, &HGMenu::LogMeIn);

    breakTab = new WBreak*[3];
    for (int i = 0; i < 3; i++)
        breakTab[i] = new WBreak();

    addWidget(login);
    addWidget(breakTab[0]);
    addWidget(pass);
    addWidget(breakTab[1]);
    addWidget(btnLog);
    addWidget(breakTab[2]);

    menu = new WMenu(menuContents, Wt::Vertical, this);
    menu->setRenderAsList(false);

    for (int i = 0; i < MENU_SLOT_COUNT; ++i)
        menuSlots[i] = NULL;

    menuSlots[MENU_SLOT_HOME] = new HGMenuOption(MENU_SLOT_HOME);
    menuSlots[MENU_SLOT_HOME]->AddMenuItem(LVL_NOT_LOGGED, session, TXT_MENU_HOME, new DefaultPage(sess));
    menuSlots[MENU_SLOT_HOME]->AddMenuItem(LVL_PLAYER, TXT_MENU_HOME, menuSlots[MENU_SLOT_HOME]->GetMenuItemForLevel(LVL_NOT_LOGGED));

    menuSlots[MENU_SLOT_ACCOUNT] = new HGMenuOption(MENU_SLOT_ACCOUNT);
    menuSlots[MENU_SLOT_ACCOUNT]->AddMenuItem(LVL_NOT_LOGGED, session, TXT_MENU_REGISTER, new RegisterPage(sess));
    menuSlots[MENU_SLOT_ACCOUNT]->AddMenuItem(LVL_PLAYER, session, TXT_MENU_ACC_INFO, new AccountInfoPage(sess));

//    menuSlots[MENU_SLOT_PASSWORD] = new HGMenuOption(MENU_SLOT_PASSWORD);
//    menuSlots[MENU_SLOT_PASSWORD]->AddMenuItem(LVL_NOT_LOGGED, session, TXT_MENU_PASS_RECOVERY, new PasswordRecoveryPage(sess));
//    menuSlots[MENU_SLOT_PASSWORD]->AddMenuItem(LVL_PLAYER, session, TXT_MENU_PASS_CHANGE, new PasswordChangePage(sess));

//    menuSlots[MENU_SLOT_SERVER_STATUS] = new HGMenuOption(MENU_SLOT_SERVER_STATUS);
//    menuSlots[MENU_SLOT_SERVER_STATUS]->AddMenuItem(LVL_NOT_LOGGED, session, TXT_MENU_SERVER_STATUS, new ServerStatusPage(sess));



    menuSlots[MENU_SLOT_ERROR] = new HGMenuOption(MENU_SLOT_ERROR);
    //menuSlots[MENU_SLOT_ERROR]->AddMenuItem(LVL_NOT_LOGGED, session, TXT_MENU_ERROR, new ErrorPage(session));
    menuSlots[MENU_SLOT_ERROR]->AddMenuItem(LVL_NOT_LOGGED, TXT_MENU_ERROR, new WMenuItem(sess->GetText(TXT_MENU_ERROR), new ErrorPage(session)));
    menuSlots[MENU_SLOT_ERROR]->AddMenuItem(LVL_PLAYER, TXT_MENU_ERROR, menuSlots[MENU_SLOT_ERROR]->GetMenuItemForLevel(LVL_NOT_LOGGED));
    menuSlots[MENU_SLOT_ERROR]->GetMenuItemForLevel(LVL_PLAYER)->hide();
    menuSlots[MENU_SLOT_ERROR]->GetMenuItemForLevel(LVL_PLAYER)->disable();

    ShowMenuOptions();

    menu->itemSelected().connect(this, &HGMenu::RefreshActiveMenuWidget);
    addWidget(menu);
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
    for (int i = 0; i < menuContents->count(); ++i)
        menuContents->widget(i)->refresh();
}

void HGMenu::RefreshActiveMenuWidget()
{
    menuContents->currentWidget()->refresh();
}

void HGMenu::LogMeIn()
{
    Database * db = new Database();
    if (!db->Connect(SERVER_DB_DATA, SQL_REALMDB))
    {
        SetError(ERROR_SLOT_BASE, TXT_MENU_ERROR);
        std::string tmpErr = db->GetError();
        SetError(ERROR_SLOT_DB, tmpErr);
        ShowError();
        return;
    }

    // simple login check ... should be replaced with something better

    std::string escapedLogin = db->EscapeString(login->text());
    std::string escapedPass = db->EscapeString(pass->text());

                //           0            1         2     3       4       5         6       7         8
    db->SetPQuery("SELECT username, sha_pass_hash, id, gmlevel, email, joindate, last_ip, locked, expansion FROM account WHERE username = '%s' AND sha_pass_hash = SHA1(UPPER('%s:%s'))", escapedLogin.c_str(), escapedLogin.c_str(), escapedPass.c_str());
    int executeResult = db->ExecuteQuery();

    // execute will return 0 if result will be empty and -1 if there will be DB error.
    switch (executeResult)
    {
        case -1:
        {
            // if there was database error
            std::string tmpErr = db->GetError();
            ShowError(ERROR_SLOT_DB, tmpErr);
            break;
        }
        case 0:
        {
            //if wrong data
            ShowError(ERROR_SLOT_ADDITIONAL, TXT_ERROR_WRONG_LOGIN_DATA);
            ClearLogin();
            ClearPass();
            break;
        }
        default:
        {
            DatabaseRow * row = db->GetRow(0);
            if (row)
            {
                session->login = row->fields[0].GetWString();
                session->pass = row->fields[1].GetWString();
                session->accid = row->fields[2].GetUInt64();
                session->accLvl = row->fields[3].GetAccountLevel();
                session->email = row->fields[4].GetWString();
                session->joinDate = row->fields[5].GetWString();
                session->lastIp = row->fields[6].GetWString();
                session->locked = row->fields[7].GetBool();
                session->expansion = row->fields[8].GetInt();

                login->setText("");
                pass->setText("");

                login->setHidden(true);
                pass->setHidden(true);
                btnLog->setHidden(true);
                login->setDisabled(true);
                pass->setDisabled(true);
                btnLog->setDisabled(true);

                removeWidget(login);
                removeWidget(pass);
                removeWidget(btnLog);

                for (int i = 0; i < 3; ++i)
                {
                    breakTab[i]->setHidden(true);
                    breakTab[i]->setDisabled(true);
                    removeWidget(breakTab[i]);
                }

                ShowMenuOptions();
                refresh();

                WMenuItem * tmpItem = menuSlots[MENU_SLOT_ERROR]->GetMenuItemForLevel(LVL_PLAYER);
                if (tmpItem)
                    tmpItem->hide();

                menu->select(0);
                RefreshActiveMenuWidget();
            }
            else
            {
                std::string tmpErr = "ERROR: Row not found!";
                ShowError(ERROR_SLOT_BASE, tmpErr);
            }
        }
    }

    delete db;
    db = NULL;
}

void HGMenu::SetPlLang()
{
    session->language = LANG_PL;
    refresh();
}

void HGMenu::SetEngLang()
{
    session->language = LANG_EN;
    refresh();
}

void HGMenu::ShowMenuOptions()
{
    if (!menuSlots)
        return;

    WMenuItem * tmp;

    std::vector<WMenuItem*>::const_iterator itr = menu->items().begin();
    while(itr != menu->items().end())
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
    if (session->accid < 1)
    {
        if (login)
            login->setText(session->GetText(TXT_LBL_ACC_LOGIN));

        if (pass)
            pass->setText(WString("pass"));

        if (btnLog)
            btnLog->setText(session->GetText(TXT_BTN_LOGIN));
    }
}

void HGMenu::UpdateMenuOptions()
{
    HGMenuOption * tmpOption = NULL;
    for (int i = 0; i < MENU_SLOT_COUNT; ++i)
        if (tmpOption = menuSlots[i])
            tmpOption->UpdateTexts(session);

    tmpOption = NULL;

    if (login)
        login->setText(session->GetText(TXT_LBL_ACC_LOGIN));

    if (pass)
        pass->setText(WString("pass"));

    if (btnLog)
        btnLog->setText(session->GetText(TXT_BTN_LOGIN));
}

void HGMenu::refresh()
{
    UpdateMenuOptions();
    RefreshActiveMenuWidget();

    WContainerWidget::refresh();
}

void HGMenu::ClearLogin()
{
    if (login)
        login->setText("");
}

void HGMenu::ClearPass()
{
    if (pass)
        pass->setText("");
}


bool HGMenu::SetError(ErrorSlots error, std::string &msg, ErrorPage * err)
{
    #ifdef DEBUG
    printf("\nHGMenu::SetError(ErrorSlots error = %i, std::string &msg = %s, ErrorPage * err = %i)\n", error, msg.c_str(), err ? 1 : 0);
    #endif
    ErrorPage * tmpError = err;
    if (!tmpError)
    {
        WMenuItem * tmpItem = menuSlots[MENU_SLOT_ERROR]->GetMenuItemForLevel(session->accLvl);

        if (!tmpItem)
            return false;

        ErrorPage * tmpError = (ErrorPage*)(tmpItem->itemWidget());

        if (!tmpError)
            return false;
    }

    WString tmpErr = WString::fromUTF8(msg);
    tmpError->SetErrorMsg(error, tmpErr);

    return true;
}

bool HGMenu::SetError(ErrorSlots error, uint32 textId, ErrorPage * err)
{
    #ifdef DEBUG
    printf("\nHGMenu::SetError(ErrorSlots error = %i, uint32 textId = %u, ErrorPage * err = %i)\n", error, textId, err ? 1 : 0);
    #endif
    ErrorPage * tmpError = err;
    if (!tmpError)
    {
        WMenuItem * tmpItem = menuSlots[MENU_SLOT_ERROR]->GetMenuItemForLevel(session->accLvl);

        if (!tmpItem)
            return false;

        ErrorPage * tmpError = (ErrorPage*)(tmpItem->itemWidget());

        if (!tmpError)
            return false;
    }

    tmpError->SetErrorMsg(error, textId);

    return true;
}

void HGMenu::ShowError(ErrorSlots error, std::string &msg)
{
    #ifdef DEBUG
    printf("\nHGMenu::ShowError(ErrorSlots error = %i, std::string &msg = %s)\n", error, msg.c_str());
    #endif
    WMenuItem * tmpItem = menuSlots[MENU_SLOT_ERROR]->GetMenuItemForLevel(session->accLvl);

    if (!tmpItem)
        return;

    ErrorPage * tmpError = (ErrorPage*)(tmpItem->itemWidget());

    if (!tmpError)
        return;

    SetError(error, msg, tmpError);

    menu->select(tmpItem);
    RefreshActiveMenuWidget();
}

void HGMenu::ShowError(ErrorSlots error, uint32 textId)
{
    #ifdef DEBUG
    printf("\nHGMenu::ShowError(ErrorSlots error = %i, uint32 textId = %u)\n", error, textId);
    #endif
    WMenuItem * tmpItem = menuSlots[MENU_SLOT_ERROR]->GetMenuItemForLevel(session->accLvl);

    if (!tmpItem)
        return;

    ErrorPage * tmpError = (ErrorPage*)(tmpItem->itemWidget());

    if (!tmpError)
        return;

    SetError(error, textId, tmpError);

    menu->select(tmpItem);
    RefreshActiveMenuWidget();
}

void HGMenu::ShowError()
{
    #ifdef DEBUG
    printf("\nHGMenu::ShowError()\n");
    #endif
    WMenuItem * tmpItem = menuSlots[MENU_SLOT_ERROR]->GetMenuItemForLevel(session->accLvl);

    if (!tmpItem)
        return;

    ErrorPage * tmpError = (ErrorPage*)(tmpItem->itemWidget());

    if (!tmpError)
        return;

    menu->select(tmpItem);
    RefreshActiveMenuWidget();
}