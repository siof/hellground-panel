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
 * \addtogroup Passwords Management
 * \{
 *
 * \file passRecovery.cpp
 * This file contains code needed to handle password recovery.
 *
 ***********************************************/

#include "passRecovery.h"

#include <boost/algorithm/string.hpp>
#include <Wt/WBreak>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WRegExpValidator>
#include <Wt/WText>

#include "../database.h"
#include "../misc.h"

PassRecoveryPage::PassRecoveryPage(SessionInfo * sess, WContainerWidget * parent):
    WContainerWidget(parent), session(sess)
{
    txtLogin = NULL;
    txtEmail = NULL;
    btnRecover = NULL;

    CreateRecoveryPage();
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
    Misc::Console(DEBUG_CODE, "PassRecoveryPage::refresh()");

    if (isHidden() || isDisabled())
        return;

    ClearRecoveryData();

    WContainerWidget::refresh();
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
    addWidget(new WText(Wt::WString::tr(TXT_INFO_PASS_RECOVERY)));

    for (int i = 0; i < 4; ++i)
        addWidget(new WBreak());

    recoveryInfo = new WText("");
    addWidget(recoveryInfo);
    addWidget(new WBreak());
    addWidget(new WBreak());

    txtLogin = new WLineEdit();

    WRegExpValidator * validator = new WRegExpValidator(LOGIN_VALIDATOR);
    txtLogin->setValidator(validator);
    txtLogin->setEmptyText(Wt::WString::tr(TXT_ACC_LOGIN));

    addWidget(new WText(Wt::WString::tr(TXT_ACC_LOGIN)));
    addWidget(txtLogin);
    addWidget(new WBreak());

    txtEmail = new WLineEdit();
    txtEmail->setEmptyText(Wt::WString::tr(TXT_PASS_MAIL));
    validator = new WRegExpValidator("[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,4}");
    txtEmail->setValidator(validator);

    addWidget(new WText(Wt::WString::tr(TXT_PASS_MAIL)));
    addWidget(txtEmail);
    addWidget(new WBreak());
    addWidget(new WBreak());

    btnRecover = new WPushButton(Wt::WString::tr(TXT_BTN_PASS_SEND));
    addWidget(btnRecover);

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
        Misc::Log(LOG_INVALID_DATA, "User trying to recover password with invalid data ! IP: %s login: %s email: %s", session->sessionIp.toUTF8().c_str(), txtLogin->text().toUTF8().c_str(), txtEmail->text().toUTF8().c_str());
        recoveryInfo->setText(Wt::WString::tr(TXT_ERROR_VALIDATION_RECOVERY));
        return;
    }

    Database db;

    if (!db.Connect(SERVER_DB_DATA, SQL_REALMDB))
    {
        recoveryInfo->setText(Wt::WString::tr(TXT_ERROR_DB_CANT_CONNECT));
        return;
    }

    WString login, mail, pass;

    login = db.EscapeString(txtLogin->text());

    // check if account already exists
    db.SetPQuery("SELECT id, email, NOW() FROM account WHERE username = '%s'", login.toUTF8().c_str());

    switch (db.ExecuteQuery())
    {
        case DB_RESULT_ERROR:
        case DB_RESULT_EMPTY:
            AddActivityPassRecovery(false, login.toUTF8().c_str());
            recoveryInfo->setText(Wt::WString::tr(TXT_ERROR_WRONG_RECOVERY_DATA));
            ClearRecoveryData();
            return;
        default:
            break;
    }

    uint32 accId = db.GetRow()->fields[0].GetUInt32();
    WString dbMail = db.GetRow()->fields[1].GetWString();
    std::string dbDate = db.GetRow()->fields[2].GetString();

    Misc::Console(DEBUG_CODE, "void Recover(): dbMail: %s mail: %s\n", txtEmail->text().toUTF8().c_str(), dbMail.toUTF8().c_str());

    mail = boost::to_upper_copy(txtEmail->text().toUTF8());
    dbMail = boost::to_upper_copy(dbMail.toUTF8());

    Misc::Console(DEBUG_CODE, "void Recover(): uppered: dbMail: %s mail: %s\n", mail.toUTF8().c_str(), dbMail.toUTF8().c_str());

    if (mail != dbMail)
    {
        AddActivityPassRecovery(accId, false);
        recoveryInfo->setText(Wt::WString::tr(TXT_ERROR_WRONG_RECOVERY_DATA));
        ClearRecoveryData();
        return;
    }

    pass = "";

    int passLen = Misc::Irand(PASSWORD_LENGTH_MIN, PASSWORD_LENGTH_MAX);

    std::string tmpStr;

    for (int i = 0; i < passLen; ++i)
        tmpStr += (char)(Misc::Irand(PASSWORD_ASCII_START, PASSWORD_ASCII_END));

    pass = db.EscapeString(WString::fromUTF8(tmpStr));

    WString from, msg;
    from = MAIL_FROM;

    // check should be moved to other place but here will be usefull for SendMail tests ;)
    db.SetPQuery("UPDATE account SET sha_pass_hash = SHA1(UPPER('%s:%s')), sessionkey = '', s = '', v = '', locked = '0' WHERE id = '%u';", login.toUTF8().c_str(), pass.toUTF8().c_str(), accId);

    if (db.ExecuteQuery() == DB_RESULT_ERROR)
    {
        AddActivityPassRecovery(accId, false);
        recoveryInfo->setText(Wt::WString::tr(TXT_ERROR_DB_QUERY_ERROR));
        ClearRecoveryData();
        return;
    }

    msg = Wt::WString::tr(TXT_PASS_RECOVERY_MAIL).arg(dbDate).arg(session->sessionIp.toUTF8()).arg(tmpStr);

    Misc::SendMail(from, mail, Wt::WString::tr(TXT_PASS_RECOVERY_SUBJECT), msg);

    ClearRecoveryData();

    recoveryInfo->setText(Wt::WString::tr(TXT_PASS_RECOVERY_COMPLETE));

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

        if (db.ExecutePQuery("SELECT id FROM account WHERE username = '%s'", login) > DB_RESULT_EMPTY)
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
    db.ExecutePQuery("INSERT INTO Activity VALUES ('%u', NOW(), '%s', '%s', '')", id, session->sessionIp.toUTF8().c_str(), success ? TXT_ACT_RECOVERY_SUCCESS : TXT_ACT_RECOVERY_FAIL);
}

/********************************************//**
 * \}
 * \}
 *
 ***********************************************/
