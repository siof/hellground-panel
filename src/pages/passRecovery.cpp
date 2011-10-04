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
 * \addtogroup Password recovery
 * \{
 *
 * \file passRecovery.cpp
 * This file contains code needed to handle password recovery.
 *
 ***********************************************/

#include "passRecovery.h"
#include "../database.h"

PassRecoveryPage::PassRecoveryPage(SessionInfo * sess, WContainerWidget * parent):
    WContainerWidget(parent), session(sess), needCreation(true)
{
    setContentAlignment(AlignCenter|AlignTop);
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
    #ifdef DEBUG
    printf("PassRecoveryPage::refresh()");
    #endif

    if (needCreation)
        CreateRecoveryPage();
    else
        UpdateTextWidgets();

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
    addWidget(textSlots[RECOVERY_TEXT_LOGIN].GetLabel());
    addWidget(txtLogin);
    addWidget(new WBreak());

    textSlots[RECOVERY_TEXT_EMAIL].SetLabel(session, TXT_LBL_PASS_MAIL);
    txtEmail = new WLineEdit();
    addWidget(textSlots[RECOVERY_TEXT_EMAIL].GetLabel());
    addWidget(txtEmail);
    addWidget(new WBreak());
    addWidget(new WBreak());

    btnRecover = new WPushButton(session->GetText(TXT_BTN_PASS_SEND));
    addWidget(btnRecover);

    txtLogin->focussed().connect(this, &PassRecoveryPage::ClearLogin);
    txtEmail->focussed().connect(this, &PassRecoveryPage::ClearEmail);
    btnRecover->clicked().connect(this, &PassRecoveryPage::Recover);
}

/********************************************//**
 * \brief Clear login.
 *
 *  Clears login text box.
 *
 ***********************************************/

void PassRecoveryPage::ClearLogin()
{
    txtLogin->setText(WString(""));
}

/********************************************//**
 * \brief Clear email.
 *
 * Clears email text box.
 *
 ***********************************************/

void PassRecoveryPage::ClearEmail()
{
    txtEmail->setText(WString(""));
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
    Database * db = new Database(SERVER_DB_DATA, SQL_REALMDB);
    WString login, mail, pass;

    login = db->EscapeString(txtLogin->text());

    // check if account already exists
    db->SetPQuery("SELECT id, email, NOW() FROM account WHERE username = '%s'", login.toUTF8().c_str());
    if (db->ExecuteQuery() < 1)
    {
        ClearLogin();
        ClearEmail();
        textSlots[RECOVERY_TEXT_INFO].SetLabel(session, TXT_ERROR_WRONG_RECOVERY_DATA);
        return;
    }

    uint32 accId = db->GetRow()->fields[0].GetUInt32();
    WString dbMail = db->GetRow()->fields[1].GetWString();
    std::string dbDate = db->GetRow()->fields[2].GetString();

    mail = txtEmail->text();

    if (mail != dbMail)
    {
        ClearLogin();
        ClearEmail();
        textSlots[RECOVERY_TEXT_INFO].SetLabel(session, TXT_ERROR_WRONG_RECOVERY_DATA);
        return;
    }

    pass = "";

    int passLen = irand(PASSWORD_LENGTH_MIN, PASSWORD_LENGTH_MAX);

    std::string tmpStr;

    for (int i = 0; i < passLen; ++i)
        tmpStr += (char)(irand(PASSWORD_ASCII_START, PASSWORD_ASCII_END));

    pass = db->EscapeString(WString::fromUTF8(tmpStr));

    WString from, msg;
    from = MAIL_FROM;

    // check should be moved to other place but here will be usefull for SendMail tests ;)
    db->SetPQuery("UPDATE account SET sha_pass_hash = SHA1(UPPER('%s:%s')) WHERE id = %i;", login.toUTF8().c_str(), pass.toUTF8().c_str(), accId);
    db->ExecuteQuery();

    if (session->HasText(TXT_RECOVERY_MAIL))
        msg = session->GetText(TXT_RECOVERY_MAIL);
    else
        msg = "Password Recovery Mail.\n\nRecovery date: %s \nRecovery session IP: %s \nNew pass: %s";

    // set buffer size
    std::string tmpMsg = msg.toUTF8();
    std::string tmpIp = session->sessionIp.toUTF8();
    char * buffer = new char[tmpMsg.size() + dbDate.size() + tmpIp.size() + passLen + 5];

    sprintf(buffer, tmpMsg.c_str(), dbDate.c_str(), tmpIp.c_str(), tmpStr.c_str());

    msg = buffer;

    SendMail(from, mail, session->GetText(TXT_RECOVERY_SUBJECT), msg);

    delete [] buffer;
    delete db;

    ClearLogin();
    ClearEmail();
}

/********************************************//**
 * \}
 * \}
 *
 ***********************************************/
