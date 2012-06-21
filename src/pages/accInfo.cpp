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
#include "../database.h"

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
    setContentAlignment(AlignCenter|AlignTop);

    addWidget(new WText(tr(TXT_INFO_ACCOUNT)));
    addWidget(new WBreak());
    addWidget(new WBreak());

    accPageInfo = new WText("", this);
    addWidget(new WBreak());
    addWidget(new WBreak());

    tabs = new WTabWidget();
    tabs->contentsStack()->setTransitionAnimation(WAnimation(WAnimation::SlideInFromRight, WAnimation::EaseIn), true);
    addWidget(tabs);

    needCreation = true;
}

AccountInfoPage::~AccountInfoPage()
{
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
            tabs->addTab(CreateAccountInfo(), tr(TXT_ACC_TAB_INFO), WTabWidget::PreLoading);
            tabs->addTab(CreateBanInfo(), tr(TXT_ACC_TAB_BAN), WTabWidget::PreLoading);
            tabs->addTab(CreateMuteInfo(), tr(TXT_ACC_TAB_MUTE), WTabWidget::PreLoading);
            tabs->addTab(new WText("ticket test"), tr(TXT_ACC_TAB_TICKET), WTabWidget::PreLoading);
            tabs->addTab(CreateActivityInfo(), tr(TXT_ACC_TAB_ACTIVITY), WTabWidget::PreLoading);
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

    if (!realmDb.Connect(SERVER_DB_DATA, SQL_REALMDB))
    {
        accPageInfo->setText(tr(TXT_ERROR_DB_CANT_CONNECT));
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
        ((WText*)tmpWidget)->setText(GetExpansionName(tmpRow->fields[4].GetInt()));

        tmpWidget = accountInfo->elementAt(ACCINFO_SLOT_LAST_LOGIN_DATE, 1)->widget(0);
        ((WText*)tmpWidget)->setText(tmpRow->fields[2].GetWString());

        tmpWidget = accountInfo->elementAt(ACCINFO_SLOT_LAST_LOGGED_IP, 1)->widget(0);
        ((WText*)tmpWidget)->setText(tmpRow->fields[1].GetWString());

        tmpWidget = accountInfo->elementAt(ACCINFO_SLOT_IP_LOCK, 1)->widget(0);
        ((WPushButton*)tmpWidget)->setText(tr(tmpRow->fields[6].GetBool() ? TXT_GEN_YES : TXT_GEN_NO));

        tmpWidget = accountInfo->elementAt(ACCINFO_SLOT_ONLINE, 1)->widget(0);
        ((WText*)tmpWidget)->setText(tr(tmpRow->fields[3].GetBool() ? TXT_GEN_ONLINE : TXT_GEN_OFFLINE));

        tmpWidget = accountInfo->elementAt(ACCINFO_SLOT_CLIENT_VERSION, 1)->widget(0);
        ((WText*)tmpWidget)->setText(GetLocale(tmpRow->fields[5].GetInt()));

        tmpWidget = accountInfo->elementAt(ACCINFO_SLOT_VOTE_POINTS, 1)->widget(0);
        ((WText*)tmpWidget)->setText(GetFormattedString("%u", session->vote));

        tmpWidget = accountInfo->elementAt(ACCINFO_SLOT_XP_RATE, 1)->widget(0);
        ((WPushButton*)tmpWidget)->setText(tr(session->account_flags & 0x0008 ? TXT_XP_RATE_BLIZZLIKE : TXT_XP_RATE_SERVER));

/*
        tmpWidget = accountInfo->elementAt(ACCINFO_SLOT_MULTIACC, 1)->widget(0);
        ((WText*)tmpWidget)->setText();
*/

        tmpWidget = accountInfo->elementAt(ACCINFO_SLOT_ACC_BAN, 1)->widget(0);
        realmDb.SetPQuery("SELECT banreason FROM account_banned WHERE active = 1 AND id = '%u'", session->accid);
        if (realmDb.ExecuteQuery() > DB_RESULT_EMPTY)
            ((WText*)tmpWidget)->setText(tr(TXT_BAN_BANNED).arg(realmDb.GetRow()->fields[0].GetWString()));
        else
            ((WText*)tmpWidget)->setText(tr(TXT_GEN_NO));

        tmpWidget = accountInfo->elementAt(ACCINFO_SLOT_LAST_IP_BAN, 1)->widget(0);
        realmDb.SetPQuery("SELECT banreason FROM ip_banned WHERE ip = '%s'", session->lastIp.toUTF8().c_str());
        if (realmDb.ExecuteQuery() > DB_RESULT_EMPTY)
            ((WText*)tmpWidget)->setText(tr(TXT_GEN_YES));
        else
            ((WText*)tmpWidget)->setText(tr(TXT_GEN_NO));

        tmpWidget = accountInfo->elementAt(ACCINFO_SLOT_CURR_IP_BAN, 1)->widget(0);
        realmDb.SetPQuery("SELECT banreason FROM ip_banned WHERE ip = '%s'", session->sessionIp.toUTF8().c_str());
        if (realmDb.ExecuteQuery() > DB_RESULT_EMPTY)
            ((WText*)tmpWidget)->setText(tr(TXT_GEN_YES));
        else
            ((WText*)tmpWidget)->setText(tr(TXT_GEN_NO));
    }
    else
        accPageInfo->setText(tr(TXT_ERROR_DB_QUERY_ERROR));

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
    console(DEBUG_CODE, "\nCall WContainerWidget * AccountInfoPage::CreateAccountInfo()\n");
    WContainerWidget * basicInfo = new WContainerWidget();

    accountInfo = new WTable(basicInfo);

    accountInfo->elementAt(ACCINFO_SLOT_TYPE, 0)->addWidget(new WText(tr(TXT_ACC_EXPANSION)));
    accountInfo->elementAt(ACCINFO_SLOT_TYPE, 1)->addWidget(new WText(""));

    accountInfo->elementAt(ACCINFO_SLOT_CURRENT_IP, 0)->addWidget(new WText(tr(TXT_CURRENT_IP)));
    accountInfo->elementAt(ACCINFO_SLOT_CURRENT_IP, 1)->addWidget(new WText(""));

    accountInfo->elementAt(ACCINFO_SLOT_CREATE_DATE, 0)->addWidget(new WText(tr(TXT_ACC_CREATE_DATE)));
    accountInfo->elementAt(ACCINFO_SLOT_CREATE_DATE, 1)->addWidget(new WText(""));

    accountInfo->elementAt(ACCINFO_SLOT_LAST_LOGIN_DATE, 0)->addWidget(new WText(tr(TXT_ACC_LAST_LOGIN)));
    accountInfo->elementAt(ACCINFO_SLOT_LAST_LOGIN_DATE, 1)->addWidget(new WText(""));

    accountInfo->elementAt(ACCINFO_SLOT_LAST_LOGGED_IP, 0)->addWidget(new WText(tr(TXT_ACC_LAST_IP)));
    accountInfo->elementAt(ACCINFO_SLOT_LAST_LOGGED_IP, 1)->addWidget(new WText(""));

    accountInfo->elementAt(ACCINFO_SLOT_IP_LOCK, 0)->addWidget(new WText(tr(TXT_ACC_LOCK)));
    accountInfo->elementAt(ACCINFO_SLOT_IP_LOCK, 1)->addWidget(new WPushButton("IP lock"));
    ((WPushButton*)accountInfo->elementAt(ACCINFO_SLOT_IP_LOCK, 1)->widget(0))->clicked().connect(this, &AccountInfoPage::ChangeIPLock);

    accountInfo->elementAt(ACCINFO_SLOT_ONLINE, 0)->addWidget(new WText(tr(TXT_ACC_ONLINE)));
    accountInfo->elementAt(ACCINFO_SLOT_ONLINE, 1)->addWidget(new WText(""));

    accountInfo->elementAt(ACCINFO_SLOT_CLIENT_VERSION, 0)->addWidget(new WText(tr(TXT_ACC_CLIENT_VERSION)));
    accountInfo->elementAt(ACCINFO_SLOT_CLIENT_VERSION, 1)->addWidget(new WText(""));

    accountInfo->elementAt(ACCINFO_SLOT_EMAIL, 0)->addWidget(new WText(tr(TXT_ACC_MAIL)));
    accountInfo->elementAt(ACCINFO_SLOT_EMAIL, 1)->addWidget(new WText(""));

    accountInfo->elementAt(ACCINFO_SLOT_VOTE_POINTS, 0)->addWidget(new WText(tr(TXT_ACC_SUPPORT_POINTS)));
    accountInfo->elementAt(ACCINFO_SLOT_VOTE_POINTS, 1)->addWidget(new WText(""));

    accountInfo->elementAt(ACCINFO_SLOT_XP_RATE, 0)->addWidget(new WText(tr(TXT_ACC_XP_RATE)));
    accountInfo->elementAt(ACCINFO_SLOT_XP_RATE, 1)->addWidget(new WPushButton("XP rates"));
    ((WPushButton*)accountInfo->elementAt(ACCINFO_SLOT_XP_RATE, 1)->widget(0))->clicked().connect(this, &AccountInfoPage::ChangeXPRates);

    //accMultiAcc;

    accountInfo->elementAt(ACCINFO_SLOT_ACC_BAN, 0)->addWidget(new WText(tr(TXT_ACC_BAN)));
    accountInfo->elementAt(ACCINFO_SLOT_ACC_BAN, 1)->addWidget(new WText(""));

    accountInfo->elementAt(ACCINFO_SLOT_LAST_IP_BAN, 0)->addWidget(new WText(tr(TXT_ACC_BAN_IP)));
    accountInfo->elementAt(ACCINFO_SLOT_LAST_IP_BAN, 1)->addWidget(new WText(""));

    accountInfo->elementAt(ACCINFO_SLOT_CURR_IP_BAN, 0)->addWidget(new WText(tr(TXT_CURRENT_IP_BAN)));
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

    #ifdef SHOW_EMAIL_CHARACTERS_COUNT
    visible = SHOW_EMAIL_CHARACTERS_COUNT;
    #endif

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

        #ifdef EMAIL_HIDE_CHAR_COUNT
        hidden = EMAIL_HIDE_CHAR_COUNT;
        #endif

        if (hidden <= 0)
            hidden = atPlace;
        else
            hidden += visible;

        for (i = visible; i < hidden; ++i)
            hiddenMail += EMAIL_HIDE_CHAR;

        std::string hiddenDomain;

        #ifdef HIDE_EMAIL_DOMAIN

        int dotPlace = 0, showChars = 1 + (j > 4 ? 2 : 1);

        for (i = 0; i < j; ++i)
        {
            if (domain[i] == '.')
                dotPlace = i;

            if (i < showChars || (dotPlace && i >= dotPlace))
                hiddenDomain += domain[i];
            else
                hiddenDomain += EMAIL_HIDE_CHAR;
        }

        #else
        hiddenDomain = domain;
        #endif

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
    console(DEBUG_CODE, "\nAccountInfoPage::ClearPage()\n");

    needCreation = true;

    WWidget * tmpWid;
    int i;

    for (i = ACCTAB_SLOT_COUNT - 1; i >= 0; --i)
    {
        if (tmpWid = tabs->widget(i))
        {
            tabs->removeTab(tmpWid);

            switch (i)
            {
                case ACCTAB_SLOT_BASIC:
                case ACCTAB_SLOT_ACTIVITY:
                    ((WContainerWidget*)tmpWid)->clear();
                    delete tmpWid;
                    break;
                case ACCTAB_SLOT_BANS:
                case ACCTAB_SLOT_MUTE:
                    ((WTable*)tmpWid)->clear();
                    delete tmpWid;
                    break;
                default:
                    break;
            }
        }
    }

    for (i = 0; i < ACCINFO_SLOT_COUNT; ++i)
        accountInfo->clear();
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
    if (db.Connect(SERVER_DB_DATA, SQL_REALMDB))
    {
        session->locked = !session->locked;
        db.SetPQuery("UPDATE account SET locked = '%i' WHERE id = %u", session->locked, session->accid);
        if (db.ExecuteQuery() != DB_RESULT_ERROR)
        {
            ((WPushButton*)accountInfo->elementAt(ACCINFO_SLOT_IP_LOCK, 1)->widget(0))->setText(session->locked ? tr(TXT_GEN_YES) : tr(TXT_GEN_NO));
            accPageInfo->setText(tr(TXT_ACC_LOCK_IP_STATE).arg(tr(session->locked ? TXT_GEN_ON : TXT_GEN_OFF)));
        }
        else
        {
            session->locked = !session->locked;
            accPageInfo->setText(tr(TXT_ERROR_DB_QUERY_ERROR));
        }
    }
    else
       accPageInfo->setText(tr(TXT_ERROR_DB_CANT_CONNECT));

    if (db.Connect(PANEL_DB_DATA, SQL_PANELDB))
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
    if (db.Connect(SERVER_DB_DATA, SQL_REALMDB))
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
            ((WPushButton*)accountInfo->elementAt(ACCINFO_SLOT_XP_RATE, 1)->widget(0))->setText(session->account_flags & 0x0008 ? tr(TXT_XP_RATE_BLIZZLIKE) : tr(TXT_XP_RATE_SERVER));
        else
        {
            session->account_flags = prevflags;
            accPageInfo->setText(tr(TXT_ERROR_DB_QUERY_ERROR));
        }
    }
    else
       accPageInfo->setText(tr(TXT_ERROR_DB_CANT_CONNECT));

    if (db.Connect(PANEL_DB_DATA, SQL_PANELDB))
        db.ExecutePQuery("INSERT INTO Activity VALUES ('%u', NOW(), '%s', '%s', '')", session->accid, session->sessionIp.toUTF8().c_str(), TXT_ACT_XP_RATES);
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
    WTable * banInfo = new WTable();

    banInfo->setHeaderCount(1);

    banInfo->elementAt(0, ACCBANINFO_SLOT_BANDATE)->addWidget(new WText(tr(TXT_BAN_FROM)));
    banInfo->elementAt(0, ACCBANINFO_SLOT_UNBANDATE)->addWidget(new WText(tr(TXT_BAN_TO)));
    banInfo->elementAt(0, ACCBANINFO_SLOT_BANNEDBY)->addWidget(new WText(tr(TXT_BAN_BY)));
    banInfo->elementAt(0, ACCBANINFO_SLOT_BANREASON)->addWidget(new WText(tr(TXT_BAN_REASON)));
    banInfo->elementAt(0, ACCBANINFO_SLOT_PERMANENT)->addWidget(new WText(tr(TXT_BAN_PERMANENT)));
    banInfo->elementAt(0, ACCBANINFO_SLOT_ACTIVE)->addWidget(new WText(""));

    Database realmDB;

    if (!realmDB.Connect(SERVER_DB_DATA, SQL_REALMDB))
    {
        accPageInfo->setText(tr(TXT_ERROR_DB_CANT_CONNECT));
        return banInfo;
    }

    realmDB.SetPQuery("SELECT FROM_UNIXTIME(bandate), FROM_UNIXTIME(unbandate), bannedby, banreason, active FROM account_banned WHERE id = %u ORDER BY active DESC, bandate DESC", session->accid);

    int count = realmDB.ExecuteQuery();

    switch (count)
    {
        case DB_RESULT_ERROR:
            accPageInfo->setText(tr(TXT_ERROR_DB_QUERY_ERROR));
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
                    banInfo->elementAt(i, 4)->addWidget(new WText(tr(perm ? TXT_GEN_PERM : TXT_GEN_TIMED)));

                    active = (*itr)->fields[4].GetBool();
                    banInfo->elementAt(i, 5)->addWidget(new WText(tr(active ? TXT_GEN_ACTIVE : TXT_GEN_NOT_ACTIVE)));
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
    WTable * muteInfo = new WTable();

    muteInfo->setHeaderCount(1);

    muteInfo->elementAt(0, ACCMUTEINFO_SLOT_MUTEDATE)->addWidget(new WText(tr(TXT_MUTE_FROM)));
    muteInfo->elementAt(0, ACCMUTEINFO_SLOT_UNMUTEDATE)->addWidget(new WText(tr(TXT_MUTE_TO)));
    muteInfo->elementAt(0, ACCMUTEINFO_SLOT_MUTEDBY)->addWidget(new WText(tr(TXT_MUTE_BY)));
    muteInfo->elementAt(0, ACCMUTEINFO_SLOT_MUTEREASON)->addWidget(new WText(tr(TXT_MUTE_REASON)));
    muteInfo->elementAt(0, ACCMUTEINFO_SLOT_ACTIVE)->addWidget(new WText(""));

    Database realmDB;

    if (!realmDB.Connect(SERVER_DB_DATA, SQL_REALMDB))
    {
        accPageInfo->setText(tr(TXT_ERROR_DB_CANT_CONNECT));
        return muteInfo;
    }

    realmDB.SetPQuery("SELECT FROM_UNIXTIME(mutedate), FROM_UNIXTIME(unmutedate), mutedby, mutereason, active FROM account_mute WHERE id = %u ORDER BY active DESC, mutedate DESC", session->accid);

    int count = realmDB.ExecuteQuery();

    switch (count)
    {
        case DB_RESULT_ERROR:
            accPageInfo->setText(tr(TXT_ERROR_DB_QUERY_ERROR));
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
                    muteInfo->elementAt(i, 4)->addWidget(new WText(tr(active ? TXT_GEN_ACTIVE : TXT_GEN_NOT_ACTIVE)));
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
    WContainerWidget * activityInfo = new WContainerWidget();

    activityTabs = new WTabWidget(activityInfo);
    activityTabs->contentsStack()->setTransitionAnimation(WAnimation(WAnimation::SlideInFromRight, WAnimation::EaseIn), true);

    WTable * tabPanel = new WTable(), * tabServer = new WTable();

    activityTabs->addTab(tabPanel, "Panel", WTabWidget::PreLoading);
    activityTabs->addTab(tabServer, "Server", WTabWidget::PreLoading);

    tabPanel->setHeaderCount(1);
    tabServer->setHeaderCount(1);

    tabPanel->elementAt(0, 0)->addWidget(new WText(tr(TXT_ACT_DATE)));
    tabPanel->elementAt(0, 1)->addWidget(new WText(tr(TXT_ACT_IP)));
    tabPanel->elementAt(0, 2)->addWidget(new WText(tr(TXT_ACT_TEXT)));

    tabServer->elementAt(0, 0)->addWidget(new WText(tr(TXT_ACT_DATE)));
    tabServer->elementAt(0, 1)->addWidget(new WText(tr(TXT_ACT_IP)));

    // fill tables
    Database db;
    int i;

    if (db.Connect(PANEL_DB_DATA, SQL_PANELDB))
    {
        db.SetPQuery("SELECT event_date, ip, activity_id, activity_args FROM Activity WHERE account_id = %u ORDER BY event_date DESC LIMIT %i", session->accid, ACTIVITY_PANEL_LIMIT);

        switch (db.ExecuteQuery())
        {
            case DB_RESULT_ERROR:
                accPageInfo->setText(tr(TXT_ERROR_DB_QUERY_ERROR));
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
                            tabPanel->elementAt(i, 2)->addWidget(new WText(tr(txtId).arg(txt)));
                        else
                            tabPanel->elementAt(i, 2)->addWidget(new WText(tr(txtId)));
                    }
                    else
                        tabPanel->elementAt(i, 2)->addWidget(new WText(txt));
                }
            }
        }
    }

    if (db.Connect(SERVER_DB_DATA, SQL_REALMDB))
    {
        db.SetPQuery("SELECT logindate, ip FROM account_login WHERE id = %u ORDER BY logindate DESC LIMIT %i", session->accid, ACTIVITY_SERVER_LIMIT);

        switch (db.ExecuteQuery())
        {
            case DB_RESULT_ERROR:
                accPageInfo->setText(tr(TXT_ERROR_DB_QUERY_ERROR));
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
