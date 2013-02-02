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

/********************************************//**
 * \addtogroup Pages
 * \{
 *
 * \addtogroup Accounts Account Management
 * \{
 *
 * \file accInfo.cpp
 * This file contains code needed to show player account informations.
 *
 ***********************************************/

#include "accInfo.h"

#include <Wt/WBreak>
#include <Wt/WPushButton>
#include <Wt/WStackedWidget>
#include <Wt/WTable>
#include <Wt/WTabWidget>
#include <Wt/WText>

#include "../config.h"
#include "../database.h"
#include "../misc.h"
#include "../miscAccount.h"
#include "../miscClient.h"

/********************************************//**
 * \brief Creates new AccountInfoPage object.
 *
 * \param sess      Contains user session informations.
 * \param parent    Parent container widget to which should be added this widget.
 *
 ***********************************************/

AccountInfoPage::AccountInfoPage(SessionInfo * sess, WContainerWidget * parent) :
    WContainerWidget(parent)
{
    session = sess;
    needCreation = true;

    setStyleClass("page accountwidget");
}

AccountInfoPage::~AccountInfoPage()
{
    // NULL pointers
    accountInfo = NULL;
    accPageInfo = NULL;
    tabs = NULL;
    activityTabs = NULL;
    session = NULL;
}

/********************************************//**
 * \brief Overloads WContainerWidget::refresh() for automatic content change.
 *
 * This function can create new or update existing content.
 * In most situations this is used for content update ;)
 *
 ***********************************************/

void AccountInfoPage::refresh()
{
    if (isHidden() || isDisabled())
        return;

    // only logged in players can visit this page so there is no need to create/update it in other cases
    if (session->accLvl > LVL_NOT_LOGGED)
    {
        if (needCreation)
        {
            needCreation = false;
            addWidget(new WText(Wt::WString::tr(TXT_INFO_ACCOUNT)));
            addWidget(new WBreak());
            addWidget(new WBreak());

            accPageInfo = new WText("", this);
            addWidget(new WBreak());
            addWidget(new WBreak());

            tabs = new WTabWidget();
            tabs->contentsStack()->setTransitionAnimation(WAnimation(WAnimation::SlideInFromRight, WAnimation::EaseIn), true);

            tabs->addTab(CreateAccountInfo(), Wt::WString::tr(TXT_ACC_TAB_INFO)/*, WTabWidget::PreLoading*/);
            tabs->addTab(CreateBanInfo(), Wt::WString::tr(TXT_ACC_TAB_BAN)/*, WTabWidget::PreLoading*/);
            tabs->addTab(CreateMuteInfo(), Wt::WString::tr(TXT_ACC_TAB_MUTE)/*, WTabWidget::PreLoading*/);
            tabs->addTab(new WText("ticket test"), Wt::WString::tr(TXT_ACC_TAB_TICKET)/*, WTabWidget::PreLoading*/);
            tabs->addTab(CreateActivityInfo(), Wt::WString::tr(TXT_ACC_TAB_ACTIVITY)/*, WTabWidget::PreLoading*/);

            addWidget(tabs);
        }

        UpdateInformations();
    }
    else
        ClearPage();

    WContainerWidget::refresh();
}

/********************************************//**
 * \brief Update informations.
 *
 * Informations from current selected tab will be updated.
 *
 ***********************************************/

void AccountInfoPage::UpdateInformations()
{
    switch (tabs->currentIndex())
    {
        case 0:
            UpdateAccountInfo();
            break;
/*        case 3:
            UpdateTicketInfo();
            break;*/
        default:
            break;
    }
}

/********************************************//**
 * \brief Update Basic Account Informations widgets.
 *
 * Only informations update. There is no need to delete old and create new widgets.
 *
 ***********************************************/

void AccountInfoPage::UpdateAccountInfo(bool first)
{
    WWidget * tmpWidget = NULL;

    if (first)
    {
        tmpWidget = accountInfo->elementAt(ACCINFO_SLOT_CURRENT_IP, 1)->widget(0);
        ((WText*)tmpWidget)->setText(session->sessionIp);

        tmpWidget = accountInfo->elementAt(ACCINFO_SLOT_CREATE_DATE, 1)->widget(0);
        ((WText*)tmpWidget)->setText(session->joinDate);

        tmpWidget = accountInfo->elementAt(ACCINFO_SLOT_EMAIL, 1)->widget(0);
        ((WText*)tmpWidget)->setText(GetEmail());

        return;
    }

    Database realmDb;

    if (!realmDb.Connect(DB_ACCOUNTS_DATA))
    {
        accPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_CANT_CONNECT));
        return;
    }

    DatabaseRow * tmpRow;
                    //          0     1         2         3        4        5       6
    realmDb.SetPQuery("SELECT id, last_ip, last_login, online, expansion, locale, locked FROM account WHERE id = '%u'", session->accid);

    // there should be only one record in db
    if (realmDb.ExecuteQuery() > DB_RESULT_EMPTY)
    {
        tmpRow = realmDb.GetRow();

        tmpWidget = accountInfo->elementAt(ACCINFO_SLOT_TYPE, 1)->widget(0);
        ((WText*)tmpWidget)->setText(Misc::Client::GetExpansionName(tmpRow->fields[4].GetInt()));

        tmpWidget = accountInfo->elementAt(ACCINFO_SLOT_LAST_LOGIN_DATE, 1)->widget(0);
        ((WText*)tmpWidget)->setText(tmpRow->fields[2].GetWString());

        tmpWidget = accountInfo->elementAt(ACCINFO_SLOT_LAST_LOGGED_IP, 1)->widget(0);
        ((WText*)tmpWidget)->setText(tmpRow->fields[1].GetWString());

        tmpWidget = accountInfo->elementAt(ACCINFO_SLOT_IP_LOCK, 1)->widget(0);
        ((WPushButton*)tmpWidget)->setText(Wt::WString::tr(tmpRow->fields[6].GetBool() ? TXT_GEN_YES : TXT_GEN_NO));

        tmpWidget = accountInfo->elementAt(ACCINFO_SLOT_ONLINE, 1)->widget(0);
        ((WText*)tmpWidget)->setText(Wt::WString::tr(tmpRow->fields[3].GetBool() ? TXT_GEN_ONLINE : TXT_GEN_OFFLINE));

        tmpWidget = accountInfo->elementAt(ACCINFO_SLOT_CLIENT_VERSION, 1)->widget(0);
        ((WText*)tmpWidget)->setText(Misc::Client::GetLocale(tmpRow->fields[5].GetInt()));

        tmpWidget = accountInfo->elementAt(ACCINFO_SLOT_VOTE_POINTS, 1)->widget(0);
        ((WText*)tmpWidget)->setText(Misc::GetFormattedString("%u", session->vote));

        tmpWidget = accountInfo->elementAt(ACCINFO_SLOT_XP_RATE, 1)->widget(0);
        ((WPushButton*)tmpWidget)->setText(Wt::WString::tr(session->account_flags & 0x0008 ? TXT_XP_RATE_BLIZZLIKE : TXT_XP_RATE_SERVER));

/*
        tmpWidget = accountInfo->elementAt(ACCINFO_SLOT_MULTIACC, 1)->widget(0);
        ((WText*)tmpWidget)->setText();
*/

        tmpWidget = accountInfo->elementAt(ACCINFO_SLOT_ACC_BAN, 1)->widget(0);
        realmDb.SetPQuery("SELECT banreason FROM account_banned WHERE active = 1 AND id = '%u'", session->accid);
        if (realmDb.ExecuteQuery() > DB_RESULT_EMPTY)
            ((WText*)tmpWidget)->setText(Wt::WString::tr(TXT_BAN_BANNED).arg(realmDb.GetRow()->fields[0].GetWString()));
        else
            ((WText*)tmpWidget)->setText(Wt::WString::tr(TXT_GEN_NO));

        tmpWidget = accountInfo->elementAt(ACCINFO_SLOT_LAST_IP_BAN, 1)->widget(0);
        realmDb.SetPQuery("SELECT banreason FROM ip_banned WHERE ip = '%s'", session->lastIp.toUTF8().c_str());
        if (realmDb.ExecuteQuery() > DB_RESULT_EMPTY)
            ((WText*)tmpWidget)->setText(Wt::WString::tr(TXT_GEN_YES));
        else
            ((WText*)tmpWidget)->setText(Wt::WString::tr(TXT_GEN_NO));

        tmpWidget = accountInfo->elementAt(ACCINFO_SLOT_CURR_IP_BAN, 1)->widget(0);
        realmDb.SetPQuery("SELECT banreason FROM ip_banned WHERE ip = '%s'", session->sessionIp.toUTF8().c_str());
        if (realmDb.ExecuteQuery() > DB_RESULT_EMPTY)
            ((WText*)tmpWidget)->setText(Wt::WString::tr(TXT_GEN_YES));
        else
            ((WText*)tmpWidget)->setText(Wt::WString::tr(TXT_GEN_NO));
    }
    else
        accPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_QUERY_ERROR));

    tmpRow = NULL;
}

/********************************************//**
 * \brief Create informations widgets.
 *
 * Create widgets for each slot and fills them with informations.
 * This should be done only once for player.
 *
 ***********************************************/

WContainerWidget * AccountInfoPage::CreateAccountInfo()
{
    Misc::Console(DEBUG_CODE, "\nCall WContainerWidget * AccountInfoPage::CreateAccountInfo()\n");
    Wt::WContainerWidget * basicInfo = new Wt::WContainerWidget(this);

    accountInfo = new Wt::WTable(basicInfo);

    accountInfo->elementAt(ACCINFO_SLOT_TYPE, 0)->addWidget(new WText(Wt::WString::tr(TXT_ACC_EXPANSION)));
    accountInfo->elementAt(ACCINFO_SLOT_TYPE, 1)->addWidget(new WText(""));

    accountInfo->elementAt(ACCINFO_SLOT_CURRENT_IP, 0)->addWidget(new WText(Wt::WString::tr(TXT_CURRENT_IP)));
    accountInfo->elementAt(ACCINFO_SLOT_CURRENT_IP, 1)->addWidget(new WText(""));

    accountInfo->elementAt(ACCINFO_SLOT_CREATE_DATE, 0)->addWidget(new WText(Wt::WString::tr(TXT_ACC_CREATE_DATE)));
    accountInfo->elementAt(ACCINFO_SLOT_CREATE_DATE, 1)->addWidget(new WText(""));

    accountInfo->elementAt(ACCINFO_SLOT_LAST_LOGIN_DATE, 0)->addWidget(new WText(Wt::WString::tr(TXT_ACC_LAST_LOGIN)));
    accountInfo->elementAt(ACCINFO_SLOT_LAST_LOGIN_DATE, 1)->addWidget(new WText(""));

    accountInfo->elementAt(ACCINFO_SLOT_LAST_LOGGED_IP, 0)->addWidget(new WText(Wt::WString::tr(TXT_ACC_LAST_IP)));
    accountInfo->elementAt(ACCINFO_SLOT_LAST_LOGGED_IP, 1)->addWidget(new WText(""));

    accountInfo->elementAt(ACCINFO_SLOT_IP_LOCK, 0)->addWidget(new WText(Wt::WString::tr(TXT_ACC_LOCK)));
    accountInfo->elementAt(ACCINFO_SLOT_IP_LOCK, 1)->addWidget(new WPushButton("IP lock"));
    ((WPushButton*)accountInfo->elementAt(ACCINFO_SLOT_IP_LOCK, 1)->widget(0))->clicked().connect(this, &AccountInfoPage::ChangeIPLock);

    accountInfo->elementAt(ACCINFO_SLOT_ONLINE, 0)->addWidget(new WText(Wt::WString::tr(TXT_ACC_ONLINE)));
    accountInfo->elementAt(ACCINFO_SLOT_ONLINE, 1)->addWidget(new WText(""));

    accountInfo->elementAt(ACCINFO_SLOT_CLIENT_VERSION, 0)->addWidget(new WText(Wt::WString::tr(TXT_ACC_CLIENT_VERSION)));
    accountInfo->elementAt(ACCINFO_SLOT_CLIENT_VERSION, 1)->addWidget(new WText(""));

    accountInfo->elementAt(ACCINFO_SLOT_EMAIL, 0)->addWidget(new WText(Wt::WString::tr(TXT_ACC_MAIL)));
    accountInfo->elementAt(ACCINFO_SLOT_EMAIL, 1)->addWidget(new WText(""));

    accountInfo->elementAt(ACCINFO_SLOT_VOTE_POINTS, 0)->addWidget(new WText(Wt::WString::tr(TXT_ACC_SUPPORT_POINTS)));
    accountInfo->elementAt(ACCINFO_SLOT_VOTE_POINTS, 1)->addWidget(new WText(""));

    accountInfo->elementAt(ACCINFO_SLOT_XP_RATE, 0)->addWidget(new WText(Wt::WString::tr(TXT_ACC_XP_RATE)));
    accountInfo->elementAt(ACCINFO_SLOT_XP_RATE, 1)->addWidget(new WPushButton("XP rates"));
    ((WPushButton*)accountInfo->elementAt(ACCINFO_SLOT_XP_RATE, 1)->widget(0))->clicked().connect(this, &AccountInfoPage::ChangeXPRates);

    //accMultiAcc;

    accountInfo->elementAt(ACCINFO_SLOT_ACC_BAN, 0)->addWidget(new WText(Wt::WString::tr(TXT_ACC_BAN)));
    accountInfo->elementAt(ACCINFO_SLOT_ACC_BAN, 1)->addWidget(new WText(""));

    accountInfo->elementAt(ACCINFO_SLOT_LAST_IP_BAN, 0)->addWidget(new WText(Wt::WString::tr(TXT_ACC_BAN_IP)));
    accountInfo->elementAt(ACCINFO_SLOT_LAST_IP_BAN, 1)->addWidget(new WText(""));

    accountInfo->elementAt(ACCINFO_SLOT_CURR_IP_BAN, 0)->addWidget(new WText(Wt::WString::tr(TXT_CURRENT_IP_BAN)));
    accountInfo->elementAt(ACCINFO_SLOT_CURR_IP_BAN, 1)->addWidget(new WText(""));

    UpdateAccountInfo(true);

    return basicInfo;
}

/********************************************//**
 * \brief Return account email
 *
 * \return Email address
 *
 * This function returns account email address which is stored in session.
 * Email can be full or partially hidden depends on config settings.
 * If there will be error then email will be empty string.
 *
 ***********************************************/

WString AccountInfoPage::GetEmail()
{
    int visible = 0;
    WString tmpStr;

    if (session->accLvl < LVL_PLAYER)
        return tmpStr;

    visible = sConfig.GetConfig(CONFIG_EMAIL_SHOW_CHAR_COUNT);

    if (visible)
    {
        std::string tmpEmail = session->email.toUTF8();
        int count = tmpEmail.size();
        char * domain;
        int atPlace = 0, i, j, hidden = 0;

        for (i = 0; i < count; ++i)
        {
            if (tmpEmail[i] == '@')
            {
                atPlace = i;
                break;
            }
        }

        if (!atPlace || atPlace >= count)
            return tmpStr;

        if (visible > atPlace)
            visible = atPlace;

        domain = new char[count - atPlace];
        j = 0;

        for (i = atPlace; i < count; ++i, ++j)
            domain[j] = tmpEmail[i];

        std::string hiddenMail;

        for (i = 0; i < visible; ++i)
            hiddenMail += tmpEmail[i];

        hidden = sConfig.GetConfig(CONFIG_EMAIL_HIDE_CHAR_COUNT);

        if (hidden <= 0)
            hidden = atPlace;
        else
            hidden += visible;

        std::string hideChar = sConfig.GetConfig(CONFIG_EMAIL_HIDE_CHARACTER);

        for (i = visible; i < hidden; ++i)
            hiddenMail += hideChar;

        std::string hiddenDomain;

        if (sConfig.GetConfig(CONFIG_EMAIL_HIDE_DOMAIN))
        {
            int dotPlace = 0, showChars = 1 + (j > 4 ? 2 : 1);

            for (i = 0; i < j; ++i)
            {
                if (domain[i] == '.')
                    dotPlace = i;

                if (i < showChars || (dotPlace && i >= dotPlace))
                    hiddenDomain += domain[i];
                else
                    hiddenDomain += hideChar;
            }
        }
        else
            hiddenDomain = domain;

        hiddenMail += hiddenDomain;

        tmpStr = WString::fromUTF8(hiddenMail);

        delete [] domain;
    }
    else
        tmpStr = session->email;

    return tmpStr;
}

/********************************************//**
 * \brief Clears informations in widgets.
 *
 * Clears informations in widgets when current account level is too low.
 *
 ***********************************************/

void AccountInfoPage::ClearPage()
{
    Misc::Console(DEBUG_CODE, "\nAccountInfoPage::ClearPage()\n");

    needCreation = true;

    clear();

    // NULL pointers
    accountInfo = NULL;
    accPageInfo = NULL;
    tabs = NULL;
    activityTabs = NULL;
}

/********************************************//**
 * \brief Changes IP Lock for account
 *
 * Function to change current IP lock state in session and DB.
 *
 ***********************************************/

void AccountInfoPage::ChangeIPLock()
{
    if (session->accLvl < LVL_PLAYER)
        return;

    Database db;
    if (db.Connect(DB_ACCOUNTS_DATA))
    {
        session->locked = !session->locked;
        db.SetPQuery("UPDATE account SET locked = '%i' WHERE id = %u", session->locked, session->accid);
        if (db.ExecuteQuery() != DB_RESULT_ERROR)
        {
            ((WPushButton*)accountInfo->elementAt(ACCINFO_SLOT_IP_LOCK, 1)->widget(0))->setText(Wt::WString::tr(session->locked ? TXT_GEN_YES : TXT_GEN_NO));
            accPageInfo->setText(Wt::WString::tr(TXT_ACC_LOCK_IP_STATE).arg(Wt::WString::tr(session->locked ? TXT_GEN_ON : TXT_GEN_OFF)));
        }
        else
        {
            session->locked = !session->locked;
            accPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_QUERY_ERROR));
        }
    }
    else
       accPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_CANT_CONNECT));

    if (db.Connect(DB_PANEL_DATA))
        db.ExecutePQuery("INSERT INTO Activity VALUES ('%u', NOW(), '%s', '%s', '')", session->accid, session->sessionIp.toUTF8().c_str(), TXT_ACT_IP_LOCK);
}

/********************************************//**
 * \brief Changes XP Rates for account
 *
 * Function to change current XP rates state in session and DB.
 *
 ***********************************************/

void AccountInfoPage::ChangeXPRates()
{
    if (session->accLvl < LVL_PLAYER)
        return;

    Database db;
    if (db.Connect(DB_ACCOUNTS_DATA))
    {
        uint64 prevflags = session->account_flags;
        if (session->account_flags & 0x0008)
        {
            session->account_flags &= ~0x0008;
            db.SetPQuery("UPDATE account SET account_flags = account_flags &~ 0x0008 WHERE id = '%u'", session->accid);
        }
        else
        {
            session->account_flags |= 0x0008;
            db.SetPQuery("UPDATE account SET account_flags = account_flags | 0x0008 WHERE id = '%u'", session->accid);
        }

        if (db.ExecuteQuery() != DB_RESULT_ERROR)
            ((WPushButton*)accountInfo->elementAt(ACCINFO_SLOT_XP_RATE, 1)->widget(0))->setText(Wt::WString::tr(session->account_flags & 0x0008 ? TXT_XP_RATE_BLIZZLIKE : TXT_XP_RATE_SERVER));
        else
        {
            session->account_flags = prevflags;
            accPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_QUERY_ERROR));
        }
    }
    else
       accPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_CANT_CONNECT));

    Misc::Account::AddActivity(session->accid, session->sessionIp.toUTF8().c_str(), TXT_ACT_XP_RATES, "");
}

/********************************************//**
 * \brief Creates ban informations
 *
 * All bans (also expired) on account will be listed.
 * I think there is no need to update this informations...
 *
 ***********************************************/

WTable * AccountInfoPage::CreateBanInfo()
{
    WTable * banInfo = new WTable(this);

    banInfo->setHeaderCount(1);

    banInfo->elementAt(0, ACCBANINFO_SLOT_BANDATE)->addWidget(new WText(Wt::WString::tr(TXT_BAN_FROM)));
    banInfo->elementAt(0, ACCBANINFO_SLOT_UNBANDATE)->addWidget(new WText(Wt::WString::tr(TXT_BAN_TO)));
    banInfo->elementAt(0, ACCBANINFO_SLOT_BANNEDBY)->addWidget(new WText(Wt::WString::tr(TXT_BAN_BY)));
    banInfo->elementAt(0, ACCBANINFO_SLOT_BANREASON)->addWidget(new WText(Wt::WString::tr(TXT_BAN_REASON)));
    banInfo->elementAt(0, ACCBANINFO_SLOT_PERMANENT)->addWidget(new WText(Wt::WString::tr(TXT_BAN_PERMANENT)));
    banInfo->elementAt(0, ACCBANINFO_SLOT_ACTIVE)->addWidget(new WText(""));

    Database realmDB;

    if (!realmDB.Connect(DB_ACCOUNTS_DATA))
    {
        accPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_CANT_CONNECT));
        return banInfo;
    }

    realmDB.SetPQuery("SELECT FROM_UNIXTIME(bandate), FROM_UNIXTIME(unbandate), bannedby, banreason, active FROM account_banned WHERE id = %u ORDER BY active DESC, bandate DESC", session->accid);

    switch (realmDB.ExecuteQuery())
    {
        case DB_RESULT_ERROR:
            accPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_QUERY_ERROR));
            break;
        case DB_RESULT_EMPTY:
//            accPageInfo->setText(tr(TXT_NEVER_BANNED));
            break;
        default:
            {
                int i = 1, j;
                bool active;
                bool perm;
                std::list<DatabaseRow*> rows = realmDB.GetRows();
                realmDB.Disconnect();

                for (std::list<DatabaseRow*>::const_iterator itr = rows.begin(); itr != rows.end(); ++itr, ++i)
                {
                    for (j = 0; j < 4; ++j)
                        banInfo->elementAt(i, j)->addWidget(new WText((*itr)->fields[j].GetWString()));

                    perm = (*itr)->fields[0].GetWString() == (*itr)->fields[1].GetWString();
                    banInfo->elementAt(i, 4)->addWidget(new WText(Wt::WString::tr(perm ? TXT_GEN_PERM : TXT_GEN_TIMED)));

                    active = (*itr)->fields[4].GetBool();
                    banInfo->elementAt(i, 5)->addWidget(new WText(Wt::WString::tr(active ? TXT_GEN_ACTIVE : TXT_GEN_NOT_ACTIVE)));
                }
            }
            break;
    }

    return banInfo;
}

/********************************************//**
 * \brief Creates account mute informations
 *
 * All mute (also expired) on account will be listed.
 * I think there is no need to update this informations...
 *
 ***********************************************/

WTable * AccountInfoPage::CreateMuteInfo()
{
    WTable * muteInfo = new WTable(this);

    muteInfo->setHeaderCount(1);

    muteInfo->elementAt(0, ACCMUTEINFO_SLOT_MUTEDATE)->addWidget(new WText(Wt::WString::tr(TXT_MUTE_FROM)));
    muteInfo->elementAt(0, ACCMUTEINFO_SLOT_UNMUTEDATE)->addWidget(new WText(Wt::WString::tr(TXT_MUTE_TO)));
    muteInfo->elementAt(0, ACCMUTEINFO_SLOT_MUTEDBY)->addWidget(new WText(Wt::WString::tr(TXT_MUTE_BY)));
    muteInfo->elementAt(0, ACCMUTEINFO_SLOT_MUTEREASON)->addWidget(new WText(Wt::WString::tr(TXT_MUTE_REASON)));
    muteInfo->elementAt(0, ACCMUTEINFO_SLOT_ACTIVE)->addWidget(new WText(""));

    Database realmDB;

    if (!realmDB.Connect(DB_ACCOUNTS_DATA))
    {
        accPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_CANT_CONNECT));
        return muteInfo;
    }

    realmDB.SetPQuery("SELECT FROM_UNIXTIME(mutedate), FROM_UNIXTIME(unmutedate), mutedby, mutereason, active FROM account_mute WHERE id = %u ORDER BY active DESC, mutedate DESC", session->accid);

    int count = realmDB.ExecuteQuery();

    switch (count)
    {
        case DB_RESULT_ERROR:
            accPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_QUERY_ERROR));
            break;
        case DB_RESULT_EMPTY:
//            accPageInfo->setText(tr(TXT_NEVER_MUTED));
            break;
        default:
            {
                int i = 1, j;
                bool active;
                std::list<DatabaseRow*> rows = realmDB.GetRows();
                realmDB.Disconnect();

                for (std::list<DatabaseRow*>::const_iterator itr = rows.begin(); itr != rows.end(); ++itr, ++i)
                {
                    for (j = 0; j < 4; ++j)
                        muteInfo->elementAt(i, j)->addWidget(new WText((*itr)->fields[j].GetWString()));

                    active = (*itr)->fields[4].GetBool();
                    muteInfo->elementAt(i, 4)->addWidget(new WText(Wt::WString::tr(active ? TXT_GEN_ACTIVE : TXT_GEN_NOT_ACTIVE)));
                }
            }
            break;
    }

    return muteInfo;
}

/********************************************//**
 * \brief Creates account activity informations
 *
 * Lists saved activities for account. Count to list
 * is provided by config file.
 *
 ***********************************************/

WContainerWidget * AccountInfoPage::CreateActivityInfo()
{
    WContainerWidget * activityInfo = new WContainerWidget(this);

    activityTabs = new WTabWidget(activityInfo);
    activityTabs->contentsStack()->setTransitionAnimation(WAnimation(WAnimation::SlideInFromRight, WAnimation::EaseIn), true);

    WTable * tabPanel = new WTable(), * tabServer = new WTable();

    activityTabs->addTab(tabPanel, "Panel", WTabWidget::PreLoading);
    activityTabs->addTab(tabServer, "Server", WTabWidget::PreLoading);

    tabPanel->setHeaderCount(1);
    tabServer->setHeaderCount(1);

    tabPanel->elementAt(0, 0)->addWidget(new WText(Wt::WString::tr(TXT_ACT_DATE)));
    tabPanel->elementAt(0, 1)->addWidget(new WText(Wt::WString::tr(TXT_ACT_IP)));
    tabPanel->elementAt(0, 2)->addWidget(new WText(Wt::WString::tr(TXT_ACT_TEXT)));

    tabServer->elementAt(0, 0)->addWidget(new WText(Wt::WString::tr(TXT_ACT_DATE)));
    tabServer->elementAt(0, 1)->addWidget(new WText(Wt::WString::tr(TXT_ACT_IP)));

    // fill tables
    Database db;
    int i;

    if (db.Connect(DB_PANEL_DATA))
    {
        db.SetPQuery("SELECT event_date, ip, activity_id, activity_args FROM Activity WHERE account_id = %u ORDER BY event_date DESC LIMIT %i", session->accid, sConfig.GetConfig(CONFIG_ACTIVITY_LIMIT_PANEL));

        switch (db.ExecuteQuery())
        {
            case DB_RESULT_ERROR:
                accPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_QUERY_ERROR));
                break;
            case DB_RESULT_EMPTY:
                break;
            default:
            {
                DatabaseRow * tmpRow;
                std::list<DatabaseRow*> rows = db.GetRows();
                db.Disconnect();
                i = 1;

                for (std::list<DatabaseRow*>::const_iterator itr = rows.begin(); itr != rows.end(); ++itr, ++i)
                {
                    tmpRow = *itr;

                    std::string txtId = tmpRow->fields[2].GetString();
                    WString txt = tmpRow->fields[3].GetWString();

                    tabPanel->elementAt(i, 0)->addWidget(new WText(tmpRow->fields[0].GetWString()));
                    tabPanel->elementAt(i, 1)->addWidget(new WText(tmpRow->fields[1].GetWString()));

                    if (!txtId.empty())
                    {
                        if (txt != "")
                            tabPanel->elementAt(i, 2)->addWidget(new WText(Wt::WString::tr(txtId).arg(txt)));
                        else
                            tabPanel->elementAt(i, 2)->addWidget(new WText(Wt::WString::tr(txtId)));
                    }
                    else
                        tabPanel->elementAt(i, 2)->addWidget(new WText(txt));
                }
            }
        }
    }

    if (db.Connect(DB_ACCOUNTS_DATA))
    {
        db.SetPQuery("SELECT logindate, ip FROM account_login WHERE id = %u ORDER BY logindate DESC LIMIT %i", session->accid, sConfig.GetConfig(CONFIG_ACTIVITY_LIMIT_SERVER));

        switch (db.ExecuteQuery())
        {
            case DB_RESULT_ERROR:
                accPageInfo->setText(Wt::WString::tr(TXT_ERROR_DB_QUERY_ERROR));
                break;
            case DB_RESULT_EMPTY:
                break;
            default:
            {
                DatabaseRow * tmpRow;
                std::list<DatabaseRow*> rows = db.GetRows();
                db.Disconnect();
                i = 1;

                for (std::list<DatabaseRow*>::const_iterator itr = rows.begin(); itr != rows.end(); ++itr, ++i)
                {
                    tmpRow = *itr;
                    tabServer->elementAt(i, 0)->addWidget(new WText(tmpRow->fields[0].GetWString()));
                    tabServer->elementAt(i, 1)->addWidget(new WText(tmpRow->fields[1].GetWString()));
                }
            }
        }
    }

    return activityInfo;
}

/********************************************//**
 * \}
 * \}
 *
 ***********************************************/
