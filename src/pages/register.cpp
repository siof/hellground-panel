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
 * \file register.cpp
 * This file contains code needed to handle player registration.
 *
 ***********************************************/

#include "register.h"

#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WBreak>
#include <Wt/WCheckBox>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WRegExpValidator>
#include <Wt/WText>

#include "../config.h"
#include "../database.h"
#include "../misc.h"
#include "../miscAccount.h"
#include "../miscHash.h"

RegisterPage::RegisterPage(SessionInfo * sess, WContainerWidget * parent):
    WContainerWidget(parent), session(sess)
{
    txtLogin = NULL;
    txtEmail = NULL;
    btnRegister = NULL;
    chRules = NULL;

    CreateRegisterPage();
    setStyleClass("page registerwidget");
}

RegisterPage::~RegisterPage()
{

}

/********************************************//**
 * \brief Overloads WContainerWidget::refresh() for automatic content change.
 *
 * This function can create new or update existing content.
 * In most situations this is used for content update ;)
 *
 ***********************************************/

void RegisterPage::refresh()
{
    Misc::Console(DEBUG_CODE, "RegisterPage::refresh()");
    WContainerWidget::refresh();
}

/********************************************//**
 * \brief Create register page and it's widgets.
 *
 * Create widgets for each slot (and additional not sloted widgets) and fills labels with text.
 * This should be done only once for player.
 *
 ***********************************************/

void RegisterPage::CreateRegisterPage()
{
    addWidget(new WText(Wt::WString::tr(TXT_INFO_REGISTRATION)));
    for (int i = 0; i < 4; ++i)
        addWidget(new Wt::WBreak());

    regInfo = new Wt::WText("");
    addWidget(regInfo);
    addWidget(new Wt::WBreak());
    addWidget(new Wt::WBreak());

    if (!wApp->environment().javaScript() || !wApp->environment().ajax())
    {
        regInfo->setText(Wt::WString::tr(TXT_ERROR_NEED_JAVA_SCRIPT));
        return;
    }

    txtLogin = new Wt::WLineEdit();
    txtLogin->setEmptyText(Wt::WString::tr(TXT_ACC_LOGIN));
    WRegExpValidator * validator = new Wt::WRegExpValidator(sConfig.GetConfig(CONFIG_LOGIN_VALIDATOR));
    validator->setMandatory(true);
    txtLogin->setValidator(validator);

    addWidget(new Wt::WText(Wt::WString::tr(TXT_ACC_LOGIN)));
    addWidget(txtLogin);
    addWidget(new Wt::WBreak());

    txtEmail = new Wt::WLineEdit();
    txtEmail->setEmptyText(Wt::WString::tr(TXT_ACC_MAIL));
    validator = new Wt::WRegExpValidator("[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,4}");
    validator->setMandatory(true);
    txtEmail->setValidator(validator);

    addWidget(new Wt::WText(Wt::WString::tr(TXT_ACC_MAIL)));
    addWidget(txtEmail);
    addWidget(new Wt::WBreak());

    chRules = new Wt::WCheckBox(Wt::WString::tr(TXT_REG_RULES));
    addWidget(chRules);
    addWidget(new Wt::WBreak());
    addWidget(new Wt::WBreak());

    btnRegister = new Wt::WPushButton(tr(TXT_BTN_REGISTER));

    if (wApp->environment().javaScript())
        btnRegister->setDisabled(true);

    addWidget(btnRegister);

    chRules->changed().connect(this, &RegisterPage::CheckChange);
    btnRegister->clicked().connect(this, &RegisterPage::Register);
}

/********************************************//**
 * \brief Clear login and email.
 *
 *  Clears login and email text boxes.
 *
 ***********************************************/

void RegisterPage::ClearRegisterData()
{
    txtLogin->setText("");
    txtEmail->setText("");
}

/********************************************//**
 * \brief Check change.
 *
 * Enables/disables register button depends on accepting rules.
 *
 ***********************************************/

void RegisterPage::CheckChange()
{
    btnRegister->setEnabled(chRules->isChecked());
}

/********************************************//**
 * \brief Registers account
 *
 * Function to validate and register new account
 * with randomly generated password if submitted data are valid.
 *
 ***********************************************/

void RegisterPage::Register()
{
    if (!wApp->environment().javaScript() || !wApp->environment().ajax())
        return;

    bool validLogin = txtLogin->validate() == WValidator::Valid;
    bool validEmail = txtEmail->validate() == WValidator::Valid;
    if (!validLogin || !validEmail)
    {
        Misc::Log(LOG_INVALID_DATA, "User trying to register with invalid data ! IP: %s login: %s email: %s", session->sessionIp.toUTF8().c_str(), txtLogin->text().toUTF8().c_str(), txtEmail->text().toUTF8().c_str());
        regInfo->setText(Wt::WString::tr(TXT_ERROR_VALIDATION_LOGIN));
        return;
    }

    if (!chRules->isChecked())
    {
        regInfo->setText(Wt::WString::tr(TXT_REG_RULES_NOT_ACCEPTED));
        return;
    }

    Database db;

    if (!db.Connect(DB_ACCOUNTS_DATA))
    {
        regInfo->setText(Wt::WString::tr(TXT_ERROR_DB_CANT_CONNECT));
        ClearRegisterData();
        return;
    }

    WString login, mail, password, escapedPass;

    login = db.EscapeString(txtLogin->text());

    // check if account already exists
    db.SetPQuery("SELECT account_id FROM account WHERE username = '%s'", login.toUTF8().c_str());
    if (db.ExecuteQuery() > DB_RESULT_EMPTY)
    {
        ClearRegisterData();
        regInfo->setText(Wt::WString::tr(TXT_REG_ACC_EXISTS));
        chRules->setChecked(false);
        CheckChange();
        return;
    }

    mail = txtEmail->text();
    password = Misc::Account::GeneratePassword();
    escapedPass = db.EscapeString(password);

    WString from, msg, sub, passHash;
    from = sConfig.GetConfig(CONFIG_MAIL_FROM);

    passHash = Misc::Hash::PWGetSHA1("%s:%s", Misc::Hash::HASH_FLAG_UPPER, login.toUTF8().c_str(), escapedPass.toUTF8().c_str());

    db.SetPQuery("INSERT INTO account (username, email, pass_hash, expansion_id) VALUES (UPPER('%s'), UPPER('%s'), '%s', '%i')",
                 login.toUTF8().c_str(), mail.toUTF8().c_str(), passHash.toUTF8().c_str(), sConfig.GetConfig(CONFIG_STARTING_EXPANSION));

    if (db.ExecuteQuery() == DB_RESULT_ERROR)
    {
        regInfo->setText(Wt::WString::tr(TXT_REG_ERROR));
        chRules->setChecked(false);
        CheckChange();
        ClearRegisterData();
        return;
    }

    sub = Wt::WString::tr(TXT_REG_SUBJECT);
    msg = Wt::WString::tr(TXT_REG_MAIL).arg(login.toUTF8()).arg(password.toUTF8());

    Misc::SendMail(from, mail, sub, msg);

    ClearRegisterData();
    chRules->setChecked(false);
    CheckChange();

    regInfo->setText(Wt::WString::tr(TXT_REG_COMPLETE));

    uint32 accId;

    if (db.ExecutePQuery("SELECT account_id FROM account WHERE username = '%s'", login.toUTF8().c_str()) > DB_RESULT_EMPTY)
        accId = db.GetRow()->fields[0].GetUInt32();
    else
        return;

    if (db.Connect(DB_PANEL_DATA))
        db.ExecutePQuery("INSERT INTO Activity VALUES ('%u', NOW(), '%s', '%s', '')", accId, session->sessionIp.toUTF8().c_str(), TXT_ACT_REGISTRATION_COMPLETE);
}

/********************************************//**
 * \}
 * \}
 *
 ***********************************************/
