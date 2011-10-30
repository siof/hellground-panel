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
 * \addtogroup Passwords Management
 * \{
 *
 * \file passRecovery.cpp
 * This file contains code needed to handle password recovery.
 *
 ***********************************************/

#include "passRecovery.h"
#include "../database.h"
#include <WRegExpValidator>
#include <boost/algorithm/string.hpp>

PassRecoveryPage::PassRecoveryPage(SessionInfo * sess, WContainerWidget * parent):
    WContainerWidget(parent), session(sess), needCreation(true)
{
    setContentAlignment(AlignCenter|AlignTop);

    txtLogin = NULL;
    txtEmail = NULL;
    btnRecover = NULL;
}

PassRecoveryPage::~PassRecoveryPage()
{

}

/********************************************//**
 * \brief Overloads WContainerWidget::refresh() for automatic content change.
 *
 * This function can create new or update existing content.
 * In most situations this is used for content update ;)
 *
 ***********************************************/

void PassRecoveryPage::refresh()
{
    console(DEBUG_CODE, "PassRecoveryPage::refresh()");

    // pass recovery page should be visible only for not logged yet players so there is no need to update/create it in other cases
    if (session->accLvl == LVL_NOT_LOGGED)
    {
        if (needCreation)
            CreateRecoveryPage();
        else
            UpdateTextWidgets();
    }

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

void PassRecoveryPage::UpdateTextWidgets()
{
    ClearRecoveryData();

    for (int i = 0; i < RECOVERY_TEXT_SLOT_COUNT; ++i)
        textSlots[i].UpdateLabel(session);

    btnRecover->setText(session->GetText(TXT_BTN_PASS_SEND));
}

/********************************************//**
 * \brief Create register page and it's widgets.
 *
 * Create widgets for each slot (and additional not sloted widgets) and fills labels with text.
 * This should be done only once for player.
 *
 ***********************************************/

void PassRecoveryPage::CreateRecoveryPage()
{
    clear();
    needCreation = false;

    textSlots[RECOVERY_TEXT_MAIN].SetLabel(session, TXT_LBL_PASS_RECOVERY);
    addWidget(textSlots[RECOVERY_TEXT_MAIN].GetLabel());
    addWidget(new WBreak());
    addWidget(new WBreak());
    addWidget(new WBreak());
    addWidget(new WBreak());

    textSlots[RECOVERY_TEXT_INFO].SetLabel("");
    addWidget(textSlots[RECOVERY_TEXT_INFO].GetLabel());
    addWidget(new WBreak());
    addWidget(new WBreak());

    textSlots[RECOVERY_TEXT_LOGIN].SetLabel(session, TXT_LBL_ACC_LOGIN);
    txtLogin = new WLineEdit();

    WRegExpValidator * validator = new WRegExpValidator(LOGIN_VALIDATOR);
    txtLogin->setValidator(validator);

    addWidget(textSlots[RECOVERY_TEXT_LOGIN].GetLabel());
    addWidget(txtLogin);
    addWidget(new WBreak());

    textSlots[RECOVERY_TEXT_EMAIL].SetLabel(session, TXT_LBL_PASS_MAIL);
    txtEmail = new WLineEdit();
    validator = new WRegExpValidator("[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,4}");
    txtEmail->setValidator(validator);

    addWidget(textSlots[RECOVERY_TEXT_EMAIL].GetLabel());
    addWidget(txtEmail);
    addWidget(new WBreak());
    addWidget(new WBreak());

    btnRecover = new WPushButton(session->GetText(TXT_BTN_PASS_SEND));
    addWidget(btnRecover);

    txtLogin->focussed().connect(this, &PassRecoveryPage::ClearWLineEdit);
    txtEmail->focussed().connect(this, &PassRecoveryPage::ClearWLineEdit);
    btnRecover->clicked().connect(this, &PassRecoveryPage::Recover);
}

/********************************************//**
 * \brief Clear login and email.
 *
 *  Clears login and email text boxes.
 *
 ***********************************************/

void PassRecoveryPage::ClearRecoveryData()
{
    txtLogin->setText("");
    txtEmail->setText("");
}

/********************************************//**
 * \brief Recover account password
 *
 * Function to validate data and generate new account password.
 * New password will be sent by email.
 *
 ***********************************************/

void PassRecoveryPage::Recover()
{
    bool validLogin = txtLogin->validate() == WValidator::Valid;
    bool validEmail = txtEmail->validate() == WValidator::Valid;

    if (!validLogin || !validEmail)
    {
        Log(LOG_INVALID_DATA, "User trying to recover password with invalid data ! IP: %s login: %s email: %s", session->sessionIp.toUTF8().c_str(), txtLogin->text().toUTF8().c_str(), txtEmail->text().toUTF8().c_str());
        textSlots[RECOVERY_TEXT_INFO].SetLabel(session, TXT_ERROR_NOT_VALID_DATA);
        return;
    }

    Database db;

    if (!db.Connect(SERVER_DB_DATA, SQL_REALMDB))
    {
        textSlots[RECOVERY_TEXT_INFO].SetLabel(session, TXT_DBERROR_CANT_CONNECT);
        return;
    }

    WString login, mail, pass;

    login = db.EscapeString(txtLogin->text());

    // check if account already exists
    db.SetPQuery("SELECT id, email, NOW() FROM account WHERE username = '%s'", login.toUTF8().c_str());

    switch (db.ExecuteQuery())
    {
        case RETURN_ERROR:
        case RETURN_EMPTY:
            AddActivityPassRecovery(false, login.toUTF8().c_str());
            textSlots[RECOVERY_TEXT_INFO].SetLabel(session, TXT_ERROR_WRONG_RECOVERY_DATA);
            ClearRecoveryData();
            return;
        default:
            break;
    }

    uint32 accId = db.GetRow()->fields[0].GetUInt32();
    WString dbMail = db.GetRow()->fields[1].GetWString();
    std::string dbDate = db.GetRow()->fields[2].GetString();

    console(DEBUG_CODE, "void Recover(): dbMail: %s mail: %s\n", txtEmail->text().toUTF8().c_str(), dbMail.toUTF8().c_str());

    mail = boost::to_upper_copy(txtEmail->text().toUTF8());
    dbMail = boost::to_upper_copy(dbMail.toUTF8());

    console(DEBUG_CODE, "void Recover(): uppered: dbMail: %s mail: %s\n", mail.toUTF8().c_str(), dbMail.toUTF8().c_str());

    if (mail != dbMail)
    {
        AddActivityPassRecovery(accId, false);
        textSlots[RECOVERY_TEXT_INFO].SetLabel(session, TXT_ERROR_WRONG_RECOVERY_DATA);
        ClearRecoveryData();
        return;
    }

    pass = "";

    int passLen = irand(PASSWORD_LENGTH_MIN, PASSWORD_LENGTH_MAX);

    std::string tmpStr;

    for (int i = 0; i < passLen; ++i)
        tmpStr += (char)(irand(PASSWORD_ASCII_START, PASSWORD_ASCII_END));

    pass = db.EscapeString(WString::fromUTF8(tmpStr));

    WString from, msg;
    from = MAIL_FROM;

    // check should be moved to other place but here will be usefull for SendMail tests ;)
    db.SetPQuery("UPDATE account SET sha_pass_hash = SHA1(UPPER('%s:%s')), sessionkey = '', s = '', v = '', locked = '0' WHERE id = '%i';", login.toUTF8().c_str(), pass.toUTF8().c_str(), accId);

    if (db.ExecuteQuery() == RETURN_ERROR)
    {
        AddActivityPassRecovery(accId, false);
        textSlots[RECOVERY_TEXT_INFO].SetLabel(session, TXT_DBERROR_QUERY_ERROR);
        ClearRecoveryData();
        return;
    }

    if (session->HasText(TXT_RECOVERY_MAIL))
        msg = session->GetText(TXT_RECOVERY_MAIL);
    else
        msg = "Password Recovery Mail.\n\nRecovery date: %s \nRecovery session IP: %s \nNew pass: %s";

    msg = GetFormattedString(msg.toUTF8().c_str(), dbDate.c_str(), session->sessionIp.toUTF8().c_str(), tmpStr.c_str());

    SendMail(from, mail, session->GetText(TXT_RECOVERY_SUBJECT), msg);

    ClearRecoveryData();

    textSlots[RECOVERY_TEXT_INFO].SetLabel(session, TXT_RECOVERY_COMPLETE);

    AddActivityPassRecovery(accId, true);
}

void PassRecoveryPage::AddActivityPassRecovery(bool success, const char * login)
{
    Database db;

    uint32 accId = session->accid;

    if (!accId)
    {
        if (!login || !db.Connect(SERVER_DB_DATA, SQL_REALMDB))
            return;

        if (db.ExecutePQuery("SELECT id FROM account WHERE username = '%s'", login) > RETURN_EMPTY)
            accId = db.GetRow()->fields[0].GetUInt32();
        else
            return;
    }

    AddActivityPassRecovery(accId, success);
}

void PassRecoveryPage::AddActivityPassRecovery(uint32 id, bool success)
{
    if (!id)
        return;

    Database db;

    db.Connect(PANEL_DB_DATA, SQL_PANELDB);
    db.ExecutePQuery("INSERT INTO Activity VALUES ('XXX', '%u', NOW(), '%s', '%u', '')", id, session->sessionIp.toUTF8().c_str(), success ? TXT_ACT_RECOVERY_SUCCESS : TXT_ACT_RECOVERY_FAIL);
}

/********************************************//**
 * \}
 * \}
 *
 ***********************************************/
