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

#include "../config.h"
#include "../database.h"
#include "../misc.h"
#include "../miscAccount.h"
#include "../miscHash.h"

PassRecoveryPage::PassRecoveryPage(SessionInfo * sess, WContainerWidget * parent):
    WContainerWidget(parent), session(sess)
{
    txtLogin = NULL;
    txtEmail = NULL;
    btnRecover = NULL;

    CreateRecoveryPage();
    setStyleClass("page passwordrecoverywidget");
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

    WRegExpValidator * validator = new WRegExpValidator(sConfig.GetConfig(CONFIG_LOGIN_VALIDATOR));
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
        Misc::Log(LOG_INVALID_DATA, "Validation: User is trying to recover password with invalid data ! IP: %s login: %s email: %s", session->sessionIp.toUTF8().c_str(), txtLogin->text().toUTF8().c_str(), txtEmail->text().toUTF8().c_str());
        recoveryInfo->setText(Wt::WString::tr(TXT_ERROR_VALIDATION_RECOVERY));
        return;
    }

    Database db;

    if (!db.Connect(DB_ACCOUNTS_DATA))
    {
        recoveryInfo->setText(Wt::WString::tr(TXT_ERROR_DB_CANT_CONNECT));
        return;
    }

    WString escapedLogin, mail, password, escapedPass, passHash;

    escapedLogin = db.EscapeString(txtLogin->text());

    // check if account already exists
    db.SetPQuery("SELECT account_id, email, NOW(), account_state_id FROM account WHERE username = '%s'", escapedLogin.toUTF8().c_str());

    switch (db.ExecuteQuery())
    {
        case DB_RESULT_ERROR:
        case DB_RESULT_EMPTY:
            Misc::Log(LOG_INVALID_DATA, "User is trying to recover password with invalid data ! IP: %s login: %s email: %s", session->sessionIp.toUTF8().c_str(), txtLogin->text().toUTF8().c_str(), txtEmail->text().toUTF8().c_str());
            recoveryInfo->setText(Wt::WString::tr(TXT_ERROR_WRONG_RECOVERY_DATA));
            ClearRecoveryData();
            return;
        default:
            break;
    }

    uint32 accId = db.GetRow()->fields[0].GetUInt32();
    Wt::WString dbMail = db.GetRow()->fields[1].GetWString();
    std::string dbDate = db.GetRow()->fields[2].GetString();
    AccountState accountState = AccountState(db.GetRow()->fields[3].GetInt());

    Misc::Console(DEBUG_CODE, "void Recover(): dbMail: %s mail: %s\n", txtEmail->text().toUTF8().c_str(), dbMail.toUTF8().c_str());

    mail = boost::to_upper_copy(txtEmail->text().toUTF8());
    dbMail = boost::to_upper_copy(dbMail.toUTF8());

    Misc::Console(DEBUG_CODE, "void Recover(): uppered: dbMail: %s mail: %s\n", mail.toUTF8().c_str(), dbMail.toUTF8().c_str());

    if (mail != dbMail)
    {
        Misc::Account::AddActivity(accId, session->sessionIp.toUTF8(), TXT_ACT_RECOVERY_FAIL, "");
        recoveryInfo->setText(Wt::WString::tr(TXT_ERROR_WRONG_RECOVERY_DATA));
        ClearRecoveryData();
        return;
    }

    password = Misc::Account::GeneratePassword();
    escapedPass = db.EscapeString(password);
    passHash = Misc::Hash::PWGetSHA1("%s:%s", Misc::Hash::HASH_FLAG_UPPER, escapedLogin.toUTF8().c_str(), escapedPass.toUTF8().c_str());

    WString from, msg;
    from = sConfig.GetConfig(CONFIG_MAIL_FROM);

    // if account was ip locked - unlock it
    if (accountState == ACCOUNT_STATE_IP_LOCKED)
        accountState = ACCOUNT_STATE_ACTIVE;

    db.SetPQuery("UPDATE account SET pass_hash = '%s', account_state_id = '%u' WHERE account_id = '%u'", passHash.toUTF8().c_str(), static_cast<uint32>(accountState), accId);

    if (db.ExecuteQuery() == DB_RESULT_ERROR)
    {
        Misc::Account::AddActivity(accId, session->sessionIp.toUTF8(), TXT_ACT_RECOVERY_FAIL, "");
        recoveryInfo->setText(Wt::WString::tr(TXT_ERROR_DB_QUERY_ERROR));
        ClearRecoveryData();
        return;
    }

    msg = Wt::WString::tr(TXT_PASS_RECOVERY_MAIL).arg(dbDate).arg(session->sessionIp.toUTF8()).arg(password.toUTF8());

    Misc::SendMail(from, mail, Wt::WString::tr(TXT_PASS_RECOVERY_SUBJECT), msg);

    ClearRecoveryData();

    recoveryInfo->setText(Wt::WString::tr(TXT_PASS_RECOVERY_COMPLETE));

    Misc::Account::AddActivity(accId, session->sessionIp.toUTF8(), TXT_ACT_RECOVERY_SUCCESS, "");
}

/********************************************//**
 * \}
 * \}
 *
 ***********************************************/
