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
#include "../menu.h"

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

    pageInfoSlots[ACCPAGEINFO_SLOT_INFO].SetLabel(new WText("", this), TXT_LBL_ACC_INFO);
    addWidget(new WBreak());
    addWidget(new WBreak());

    pageInfoSlots[ACCPAGEINFO_SLOT_ADDINFO].SetLabel(new WText("", this));
    addWidget(new WBreak());
    addWidget(new WBreak());

    tabs = new WTabWidget();
    addWidget(tabs);

    tabs->addTab(CreateAccountInfo(), sess->GetText(TXT_LBL_ACC_TAB_INFO), WTabWidget::PreLoading);
    tabs->addTab(CreateBanInfo(), sess->GetText(TXT_LBL_ACC_TAB_BAN), WTabWidget::PreLoading);
    tabs->addTab(CreateMuteInfo(), sess->GetText(TXT_LBL_ACC_TAB_MUTE), WTabWidget::PreLoading);
    tabs->addTab(new WText("ticket test"), sess->GetText(TXT_LBL_ACC_TAB_TICKET), WTabWidget::PreLoading);
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
        UpdateTextWidgets();
        UpdateInformations();
    }
    else
        ClearPage();

    WContainerWidget::refresh();
}

/********************************************//**
 * \brief Update text widgets.
 *
 * All text label widgets in all slots from current tab
 * will be updated, so if player will change language
 * then automagically labels should change too ;)
 *
 ***********************************************/

void AccountInfoPage::UpdateTextWidgets()
{
    int i;
    for (i = 0; i < ACCPAGEINFO_SLOT_COUNT; ++i)
        pageInfoSlots[i].UpdateLabel(session);

    int count = tabs->count();

    for (i = 0; i < count; ++i)
        tabs->setTabText(i, session->GetText(TXT_LBL_ACC_TAB_INFO + i));

    switch (tabs->currentIndex())
    {
        case 0:
            for (i = 0; i < ACCINFO_SLOT_COUNT; ++i)
                accInfoSlots[i].UpdateLabel(session);
            break;
        case 1:
            for (i = 0; i < ACCBANINFO_SLOT_COUNT; ++i)
                banInfoSlots[i].UpdateLabel(session);
            break;
        case 2:
            for (i = 0; i < ACCMUTEINFO_SLOT_COUNT; ++i)
                muteInfoSlots[i].UpdateLabel(session);
            break;
/*        case 3:
            for (i = 0; i < TICKETINFO_SLOT_COUNT; ++i)
                ticketInfoSlots[i].UpdateLabel(session);
            break;*/
        default:
            break;
    }
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
        tmpWidget = accInfoSlots[ACCINFO_SLOT_CURRENT_IP].GetWidget();
        ((WText*)tmpWidget)->setText(session->sessionIp);

        tmpWidget = accInfoSlots[ACCINFO_SLOT_CREATE_DATE].GetWidget();
        ((WText*)tmpWidget)->setText(session->joinDate);

        tmpWidget = accInfoSlots[ACCINFO_SLOT_EMAIL].GetWidget();
        ((WText*)tmpWidget)->setText(GetEmail());
        return;
    }

    Database realmDb;

    if (!realmDb.Connect(SERVER_DB_DATA, SQL_REALMDB))
    {
        pageInfoSlots[ACCPAGEINFO_SLOT_ADDINFO].SetLabel(session, TXT_DBERROR_CANT_CONNECT);
        return;
    }

    DatabaseRow * tmpRow;
                    //          0     1         2         3        4        5       6
    realmDb.SetPQuery("SELECT id, last_ip, last_login, online, expansion, locale, locked FROM account WHERE id = '%u'", session->accid);

    // there should be only one record in db
    if (realmDb.ExecuteQuery() > RETURN_EMPTY)
    {
        tmpRow = realmDb.GetRow();

        tmpWidget = accInfoSlots[ACCINFO_SLOT_TYPE].GetWidget();
        ((WText*)tmpWidget)->setText(GetExpansionName(session, tmpRow->fields[4].GetInt()));

        tmpWidget = accInfoSlots[ACCINFO_SLOT_LAST_LOGIN_DATE].GetWidget();
        ((WText*)tmpWidget)->setText(tmpRow->fields[2].GetWString());

        tmpWidget = accInfoSlots[ACCINFO_SLOT_LAST_LOGGED_IP].GetWidget();
        ((WText*)tmpWidget)->setText(tmpRow->fields[1].GetWString());

        tmpWidget = accInfoSlots[ACCINFO_SLOT_IP_LOCK].GetWidget();
        ((WPushButton*)tmpWidget)->setText(tmpRow->fields[6].GetBool() ? session->GetText(TXT_LBL_ACC_IP_LOCK_ON) : session->GetText(TXT_LBL_ACC_IP_LOCK_OFF));

        tmpWidget = accInfoSlots[ACCINFO_SLOT_ONLINE].GetWidget();
        ((WText*)tmpWidget)->setText(tmpRow->fields[3].GetBool() ? session->GetText(TXT_IS_ONLINE) : session->GetText(TXT_IS_OFFLINE));

        tmpWidget = accInfoSlots[ACCINFO_SLOT_CLIENT_VERSION].GetWidget();
        ((WText*)tmpWidget)->setText(GetLocale(tmpRow->fields[5].GetInt()));

/*
        tmpWidget = accInfoSlots[ACCINFO_SLOT_VOTE_POINTS].GetWidget();
        ((WText*)tmpWidget)->setText();

        tmpWidget = accInfoSlots[ACCINFO_SLOT_MULTIACC].GetWidget();
        ((WText*)tmpWidget)->setText();
*/

        tmpWidget = accInfoSlots[ACCINFO_SLOT_ACC_BAN].GetWidget();
        realmDb.SetPQuery("SELECT banreason FROM account_banned WHERE active = 1 AND id = '%u'", session->accid);
        if (realmDb.ExecuteQuery() > RETURN_EMPTY)
            ((WText*)tmpWidget)->setText(session->GetText(TXT_LBL_BAN_YES) + ": " + realmDb.GetRow()->fields[0].GetWString());
        else
            ((WText*)tmpWidget)->setText(session->GetText(TXT_LBL_BAN_NO));

        tmpWidget = accInfoSlots[ACCINFO_SLOT_LAST_IP_BAN].GetWidget();
        realmDb.SetPQuery("SELECT banreason FROM ip_banned WHERE ip = '%s'", session->lastIp.toUTF8().c_str());
        if (realmDb.ExecuteQuery() > RETURN_EMPTY)
            ((WText*)tmpWidget)->setText(session->GetText(TXT_LBL_BAN_YES));
        else
            ((WText*)tmpWidget)->setText(session->GetText(TXT_LBL_BAN_NO));

        tmpWidget = accInfoSlots[ACCINFO_SLOT_CURR_IP_BAN].GetWidget();
        realmDb.SetPQuery("SELECT banreason FROM ip_banned WHERE ip = '%s'", session->sessionIp.toUTF8().c_str());
        if (realmDb.ExecuteQuery() > RETURN_EMPTY)
            ((WText*)tmpWidget)->setText(session->GetText(TXT_LBL_BAN_YES));
        else
            ((WText*)tmpWidget)->setText(session->GetText(TXT_LBL_BAN_NO));
    }
    else
    {
        wApp->log("error") << "DB Error: errno: [" << realmDb.GetErrNo() << "] error: " << realmDb.GetError() << " | query: " << realmDb.GetQuery();
        pageInfoSlots[ACCPAGEINFO_SLOT_ADDINFO].SetLabel(session, TXT_DBERROR_QUERY_ERROR);
    }

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

    accInfoSlots[ACCINFO_SLOT_TYPE].SetAll(session, TXT_LBL_ACC_EXPANSION, new WText(""), 2);

    accInfoSlots[ACCINFO_SLOT_CURRENT_IP].SetAll(session, TXT_CURRENT_IP, new WText(""), 2);

    accInfoSlots[ACCINFO_SLOT_CREATE_DATE].SetAll(session, TXT_LBL_ACC_CREATE_DATE, new WText(""), 1);

    accInfoSlots[ACCINFO_SLOT_LAST_LOGIN_DATE].SetAll(session, TXT_LBL_ACC_LAST_LOGIN, new WText(""), 1);

    accInfoSlots[ACCINFO_SLOT_LAST_LOGGED_IP].SetAll(session, TXT_LBL_ACC_LAST_IP, new WText(""), 1);

    accInfoSlots[ACCINFO_SLOT_IP_LOCK].SetAll(session, TXT_LBL_ACC_IP_LOCK, new WPushButton("IP lock"), 1);
    ((WPushButton*)accInfoSlots[ACCINFO_SLOT_IP_LOCK].GetWidget())->clicked().connect(this, &AccountInfoPage::ChangeIPLock);

    accInfoSlots[ACCINFO_SLOT_ONLINE].SetAll(session, TXT_LBL_ACC_ONLINE, new WText(""), 1);

    accInfoSlots[ACCINFO_SLOT_CLIENT_VERSION].SetAll(session, TXT_LBL_ACC_CLIENT_VERSION, new WText(""), 1);

    accInfoSlots[ACCINFO_SLOT_EMAIL].SetAll(session, TXT_LBL_ACC_MAIL, new WText(""), 1);

    //accVotePoints;

    //accMultiAcc;

    accInfoSlots[ACCINFO_SLOT_ACC_BAN].SetAll(session, TXT_LBL_ACC_BAN, new WText(""), 1);

    accInfoSlots[ACCINFO_SLOT_LAST_IP_BAN].SetAll(session, TXT_LBL_ACC_IP_BAN, new WText(""), 1);

    accInfoSlots[ACCINFO_SLOT_CURR_IP_BAN].SetAll(session, TXT_CURRENT_IP_BAN, new WText(""), 1);

    int tmpCount;

    console(DEBUG_CODE, "\nCreateAccountInfo(): ACC_INFO_SLOT_COUNT: %i\n", ACCINFO_SLOT_COUNT);

    WWidget * tmpWidget;
    // add widgets to page
    for (int i = 0; i < ACCINFO_SLOT_COUNT; ++i)
    {
        tmpWidget = accInfoSlots[i].GetLabel();
        if (tmpWidget)
            basicInfo->addWidget(tmpWidget);

        tmpWidget = accInfoSlots[i].GetWidget();
        if (tmpWidget)
            basicInfo->addWidget(tmpWidget);

        tmpCount = accInfoSlots[i].GetBreakCount();

        console(DEBUG_CODE, "\nCreateAccountInfo(): i: %i, tmpCount: %i\n", i, tmpCount);

        for (int j = 0; j < tmpCount; ++j)
            basicInfo->addWidget(new WBreak());
    }

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

    WWidget * tmpWid;
    for (int i = 0; i < ACCINFO_SLOT_COUNT; ++i)
        if (tmpWid = accInfoSlots[i].GetWidget())
            ((WText*)tmpWid)->setText("");
}

/********************************************//**
 * \brief Changes IP Lock for account
 *
 * Function to change current IP lock state in session and DB.
 *
 ***********************************************/

void AccountInfoPage::ChangeIPLock()
{
    Database db;
    if (db.Connect(SERVER_DB_DATA, SQL_REALMDB))
    {
        session->locked = !session->locked;
        db.SetPQuery("UPDATE account SET locked = '%i' WHERE id = %u", session->locked, session->accid);
        if (db.ExecuteQuery() != RETURN_ERROR)
        {
            ((WPushButton*)accInfoSlots[ACCINFO_SLOT_IP_LOCK].GetWidget())->setText(session->locked ? session->GetText(TXT_LBL_ACC_IP_LOCK_ON) : session->GetText(TXT_LBL_ACC_IP_LOCK_OFF));
            pageInfoSlots[ACCPAGEINFO_SLOT_ADDINFO].GetLabel()->setText(session->locked ? session->GetText(TXT_IP_LOCK_ON) : session->GetText(TXT_IP_LOCK_OFF));
        }
        else
        {
            session->locked = !session->locked;
            pageInfoSlots[ACCPAGEINFO_SLOT_ADDINFO].SetLabel(session, TXT_DBERROR_QUERY_ERROR);
        }
    }
    else
       pageInfoSlots[ACCPAGEINFO_SLOT_ADDINFO].SetLabel(session, TXT_DBERROR_CANT_CONNECT);
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

    banInfoSlots[ACCBANINFO_SLOT_BANDATE].SetLabel(session, TXT_LBL_BAN_FROM);
    banInfoSlots[ACCBANINFO_SLOT_UNBANDATE].SetLabel(session, TXT_LBL_BAN_TO);
    banInfoSlots[ACCBANINFO_SLOT_BANNEDBY].SetLabel(session, TXT_LBL_BAN_BY);
    banInfoSlots[ACCBANINFO_SLOT_BANREASON].SetLabel(session, TXT_LBL_BAN_REASON);
    banInfoSlots[ACCBANINFO_SLOT_ACTIVE].SetLabel("");

    int i;
    for (i = 0; i < ACCBANINFO_SLOT_COUNT; ++i)
        banInfo->elementAt(0, i)->addWidget(banInfoSlots[i].GetLabel());

    Database realmDB;

    if (!realmDB.Connect(SERVER_DB_DATA, SQL_REALMDB))
    {
        pageInfoSlots[ACCPAGEINFO_SLOT_ADDINFO].SetLabel(session, TXT_DBERROR_CANT_CONNECT);
        return banInfo;
    }

    realmDB.SetPQuery("SELECT FROM_UNIXTIME(bandate), FROM_UNIXTIME(unbandate), bannedby, banreason, active FROM account_banned WHERE id = %i ORDER BY active DESC, bandate DESC", session->accid);

    int count = realmDB.ExecuteQuery();

    switch (count)
    {
        case RETURN_ERROR:
            pageInfoSlots[ACCPAGEINFO_SLOT_ADDINFO].SetLabel(session, TXT_DBERROR_QUERY_ERROR);
            break;
        case RETURN_EMPTY:
            break;
        default:
            {
                i = 1;
                int j;
                bool active;
                std::list<DatabaseRow*> rows = realmDB.GetRows();
                DatabaseRow * tmpRow;

                for (std::list<DatabaseRow*>::const_iterator itr = rows.begin(); itr != rows.end(); ++itr, ++i)
                {
                    for (j = 0; j < 4; ++j)
                        banInfo->elementAt(i, j)->addWidget(new WText((*itr)->fields[j].GetWString()));

                    active = tmpRow->fields[4].GetBool();
                    banInfo->elementAt(i, 4)->addWidget(new WText(session->GetText(active ? TXT_LBL_BAN_ACTIVE : TXT_LBL_BAN_NOT_ACTIVE)));
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

    muteInfoSlots[ACCMUTEINFO_SLOT_MUTEDATE].SetLabel(session, TXT_LBL_MUTE_FROM);
    muteInfoSlots[ACCMUTEINFO_SLOT_UNMUTEDATE].SetLabel(session, TXT_LBL_MUTE_TO);
    muteInfoSlots[ACCMUTEINFO_SLOT_MUTEDBY].SetLabel(session, TXT_LBL_MUTE_BY);
    muteInfoSlots[ACCMUTEINFO_SLOT_MUTEREASON].SetLabel(session, TXT_LBL_MUTE_REASON);
    muteInfoSlots[ACCMUTEINFO_SLOT_ACTIVE].SetLabel("");

    int i;
    for (i = 0; i < ACCMUTEINFO_SLOT_COUNT; ++i)
        muteInfo->elementAt(0, i)->addWidget(muteInfoSlots[i].GetLabel());

    Database realmDB;

    if (!realmDB.Connect(SERVER_DB_DATA, SQL_REALMDB))
    {
        pageInfoSlots[ACCPAGEINFO_SLOT_ADDINFO].SetLabel(session, TXT_DBERROR_CANT_CONNECT);
        return muteInfo;
    }

    realmDB.SetPQuery("SELECT FROM_UNIXTIME(mutedate), FROM_UNIXTIME(unmutedate), mutedby, mutereason, active FROM account_mute WHERE id = %i ORDER BY active DESC, mutedate DESC", session->accid);

    int count = realmDB.ExecuteQuery();

    switch (count)
    {
        case RETURN_ERROR:
            pageInfoSlots[ACCPAGEINFO_SLOT_ADDINFO].SetLabel(session, TXT_DBERROR_QUERY_ERROR);
            break;
        case RETURN_EMPTY:
            break;
        default:
            {
                i = 1;
                int j;
                bool active;
                std::list<DatabaseRow*> rows = realmDB.GetRows();
                DatabaseRow * tmpRow;

                for (std::list<DatabaseRow*>::const_iterator itr = rows.begin(); itr != rows.end(); ++itr, ++i)
                {
                    for (j = 0; j < 4; ++j)
                        muteInfo->elementAt(i, j)->addWidget(new WText((*itr)->fields[j].GetWString()));

                    active = tmpRow->fields[4].GetBool();
                    muteInfo->elementAt(i, 4)->addWidget(new WText(session->GetText(active ? TXT_LBL_MUTE_ACTIVE : TXT_LBL_MUTE_NOT_ACTIVE)));
                }
            }
            break;
    }

    return muteInfo;
}

/********************************************//**
 * \}
 * \}
 *
 ***********************************************/
