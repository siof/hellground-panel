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
    needInfoCreation = true;    /// we should create content for new page
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
        if (needInfoCreation)
            CreateAccountInfo();
        else
        {
            UpdateTextWidgets();
            UpdateAccountInfo();
        }
    }
    else
        ClearAccountInfo();

    WContainerWidget::refresh();
}

/********************************************//**
 * \brief Update text widgets.
 *
 * All text label widgets in all slots will be updated,
 * so if player will change language then automagically
 * labels should change too ;)
 *
 ***********************************************/

void AccountInfoPage::UpdateTextWidgets()
{
    for (int i = 0; i < ACCINFO_SLOT_COUNT; ++i)
        accInfoSlots[i].UpdateLabel(session);
}

/********************************************//**
 * \brief Update Account Informations widgets.
 *
 * Only informations update. There is no need to delete old and create new widgets.
 *
 ***********************************************/

void AccountInfoPage::UpdateAccountInfo()
{
    Database * realmDb = new Database(SERVER_DB_DATA, SQL_REALMDB);
    Database * charDb = new Database(SERVER_DB_DATA, SQL_CHARDB);
    DatabaseRow * tmpRow;
                    //          0     1         2         3        4        5       6
    realmDb->SetPQuery("SELECT id, last_ip, last_login, online, expansion, locale, locked FROM account WHERE id = '%u'", session->accid);

    // there should be only one record in db
    if (realmDb->ExecuteQuery() > 0)
    {
        tmpRow = realmDb->GetRow();

        WWidget * tmpWidget = NULL;

        tmpWidget = accInfoSlots[ACCINFO_SLOT_TYPE].GetWidget();
        ((WText*)tmpWidget)->setText(GetExpansionName(session, tmpRow->fields[4].GetInt()));

        tmpWidget = accInfoSlots[ACCINFO_SLOT_LAST_LOGIN_DATE].GetWidget();
        ((WText*)tmpWidget)->setText(tmpRow->fields[2].GetWString());

        tmpWidget = accInfoSlots[ACCINFO_SLOT_LAST_LOGGED_IP].GetWidget();
        ((WText*)tmpWidget)->setText(tmpRow->fields[1].GetWString());

        tmpWidget = accInfoSlots[ACCINFO_SLOT_IP_LOCK].GetWidget();
        ((WText*)tmpWidget)->setText(tmpRow->fields[6].GetBool() ? session->GetText(TXT_LBL_ACC_IP_LOCK_ON) : session->GetText(TXT_LBL_ACC_IP_LOCK_OFF));

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
        realmDb->SetPQuery("SELECT banreason FROM account_banned WHERE active = 1 AND id = '%u'", session->accid);
        if (realmDb->ExecuteQuery())
            ((WText*)tmpWidget)->setText(session->GetText(TXT_LBL_BAN_YES) + ": " + realmDb->GetRow()->fields[0].GetWString());
        else
            ((WText*)tmpWidget)->setText(session->GetText(TXT_LBL_BAN_NO));

        tmpWidget = accInfoSlots[ACCINFO_SLOT_LAST_IP_BAN].GetWidget();
        realmDb->SetPQuery("SELECT banreason FROM ip_banned WHERE ip = '%s'", session->lastIp.toUTF8().c_str());
        if (realmDb->ExecuteQuery())
            ((WText*)tmpWidget)->setText(session->GetText(TXT_LBL_BAN_YES));
        else
            ((WText*)tmpWidget)->setText(session->GetText(TXT_LBL_BAN_NO));

        tmpWidget = accInfoSlots[ACCINFO_SLOT_CURR_IP_BAN].GetWidget();
        realmDb->SetPQuery("SELECT banreason FROM ip_banned WHERE ip = '%s'", session->sessionIp.toUTF8().c_str());
        if (realmDb->ExecuteQuery())
            ((WText*)tmpWidget)->setText(session->GetText(TXT_LBL_BAN_YES));
        else
            ((WText*)tmpWidget)->setText(session->GetText(TXT_LBL_BAN_NO));
    }

    delete charDb;
    charDb = NULL;
    delete realmDb;
    realmDb = NULL;
    tmpRow = NULL;
}

/********************************************//**
 * \brief Create informations widgets.
 *
 * Create widgets for each slot and fills them with informations.
 * This should be done only once for player.
 *
 ***********************************************/

void AccountInfoPage::CreateAccountInfo()
{
    #ifdef DEBUG
    printf("\nCall void AccountInfoPage::CreateAccountInfo()\n");
    #endif
    clear();

    Database * realmDb = new Database(SERVER_DB_DATA, SQL_REALMDB);
    Database * charDb = new Database(SERVER_DB_DATA, SQL_CHARDB);
    DatabaseRow * tmpRow;
                    //          0     1         2         3        4        5       6
    realmDb->SetPQuery("SELECT id, last_ip, last_login, online, expansion, locale, locked FROM account WHERE id = '%u'", session->accid);

    // there should be only one record in db
    if (realmDb->ExecuteQuery() > 0)
    {
        #ifdef DEBUG
        printf("\nCreateAccountInfo(): Account founded\n");
        #endif
        needInfoCreation = false;
        tmpRow = realmDb->GetRow();

        WWidget * tmpWidget = NULL;

        accInfoSlots[ACCINFO_SLOT_INFO].SetAll(session, TXT_LBL_ACC_INFO, NULL, 3);

        tmpWidget = new WText(GetExpansionName(session, tmpRow->fields[4].GetInt()));
        accInfoSlots[ACCINFO_SLOT_TYPE].SetAll(session, TXT_LBL_ACC_EXPANSION, tmpWidget, 2);

        tmpWidget = new WText(session->sessionIp);
        accInfoSlots[ACCINFO_SLOT_CURRENT_IP].SetAll(session, TXT_CURRENT_IP, tmpWidget, 2);

        tmpWidget = new WText(session->joinDate);
        accInfoSlots[ACCINFO_SLOT_CREATE_DATE].SetAll(session, TXT_LBL_ACC_CREATE_DATE, tmpWidget, 1);

        tmpWidget = new WText(tmpRow->fields[2].GetWString());
        accInfoSlots[ACCINFO_SLOT_LAST_LOGIN_DATE].SetAll(session, TXT_LBL_ACC_LAST_LOGIN, tmpWidget, 1);

        tmpWidget = new WText(tmpRow->fields[1].GetWString());
        accInfoSlots[ACCINFO_SLOT_LAST_LOGGED_IP].SetAll(session, TXT_LBL_ACC_LAST_IP, tmpWidget, 1);

        tmpWidget = new WText(tmpRow->fields[6].GetBool() ? session->GetText(TXT_LBL_ACC_IP_LOCK_ON) : session->GetText(TXT_LBL_ACC_IP_LOCK_OFF));
        accInfoSlots[ACCINFO_SLOT_IP_LOCK].SetAll(session, TXT_LBL_ACC_IP_LOCK, tmpWidget, 1);

        tmpWidget = new WText(tmpRow->fields[3].GetBool() ? session->GetText(TXT_IS_ONLINE) : session->GetText(TXT_IS_OFFLINE));
        accInfoSlots[ACCINFO_SLOT_ONLINE].SetAll(session, TXT_LBL_ACC_ONLINE, tmpWidget, 1);

        tmpWidget = new WText(GetLocale(tmpRow->fields[5].GetInt()));
        accInfoSlots[ACCINFO_SLOT_CLIENT_VERSION].SetAll(session, TXT_LBL_ACC_CLIENT_VERSION, tmpWidget, 1);

        tmpWidget = new WText(GetEmail());
        accInfoSlots[ACCINFO_SLOT_EMAIL].SetAll(session, TXT_LBL_ACC_MAIL, tmpWidget, 1);

        //accVotePoints;

        //accMultiAcc;

        realmDb->SetPQuery("SELECT banreason FROM account_banned WHERE active = 1 AND id = '%u'", session->accid);
        if (realmDb->ExecuteQuery())
            tmpWidget = new WText(session->GetText(TXT_LBL_BAN_YES) + ": " + realmDb->GetRow()->fields[0].GetWString());
        else
            tmpWidget = new WText(session->GetText(TXT_LBL_BAN_NO));

        accInfoSlots[ACCINFO_SLOT_ACC_BAN].SetAll(session, TXT_LBL_ACC_BAN, tmpWidget, 1);


        realmDb->SetPQuery("SELECT banreason FROM ip_banned WHERE ip = '%s'", session->lastIp.toUTF8().c_str());
        if (realmDb->ExecuteQuery())
            tmpWidget = new WText(session->GetText(TXT_LBL_BAN_YES));
        else
            tmpWidget = new WText(session->GetText(TXT_LBL_BAN_NO));

        accInfoSlots[ACCINFO_SLOT_LAST_IP_BAN].SetAll(session, TXT_LBL_ACC_IP_BAN, tmpWidget, 1);


        realmDb->SetPQuery("SELECT banreason FROM ip_banned WHERE ip = '%s'", session->sessionIp.toUTF8().c_str());
        if (realmDb->ExecuteQuery())
            tmpWidget = new WText(session->GetText(TXT_LBL_BAN_YES));
        else
            tmpWidget = new WText(session->GetText(TXT_LBL_BAN_NO));

        accInfoSlots[ACCINFO_SLOT_CURR_IP_BAN].SetAll(session, TXT_CURRENT_IP_BAN, tmpWidget, 1);

//        accCharacters;
//        charName;
//        charClass;


        int tmpCount;

        #ifdef DEBUG
        printf("\nCreateAccountInfo(): ACC_INFO_SLOT_COUNT: %i\n", ACCINFO_SLOT_COUNT);
        #endif

        // add widgets to page
        for (int i = 0; i < ACCINFO_SLOT_COUNT; ++i)
        {
            tmpWidget = accInfoSlots[i].GetLabel();
            if (tmpWidget)
                addWidget(tmpWidget);

            tmpWidget = accInfoSlots[i].GetWidget();
            if (tmpWidget)
                addWidget(tmpWidget);

            tmpCount = accInfoSlots[i].GetBreakCount();

            #ifdef DEBUG
            printf("\nCreateAccountInfo(): i: %i, tmpCount: %i\n", i, tmpCount);
            #endif
            for (int j = 0; j < tmpCount; ++j)
                addWidget(new WBreak());
        }
    }
    else
    {
        // this should be replaced with something better ...
        HGMenu * tmpMenu = (HGMenu*)(parent()->parent());

        std::string dbError = realmDb->GetError();

        delete charDb;
        charDb = NULL;
        delete realmDb;
        realmDb = NULL;

        if (tmpMenu)
            tmpMenu->ShowError(ERROR_SLOT_DB, dbError);

        return;
    }

    delete charDb;
    charDb = NULL;
    delete realmDb;
    realmDb = NULL;
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

void AccountInfoPage::ClearAccountInfo()
{
    #ifdef DEBUG
    printf("\nAccountInfoPage::ClearAccountInfo()\n");
    #endif
    WWidget * tmpWid;
    for (int i = 0; i < ACCINFO_SLOT_COUNT; ++i)
        if (tmpWid = accInfoSlots[i].GetWidget())
            ((WText*)tmpWid)->setText("");
}

/********************************************//**
 * \}
 * \}
 *
 ***********************************************/
