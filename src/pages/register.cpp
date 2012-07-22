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

#include "../database.h"

RegisterPage::RegisterPage(SessionInfo * sess, WContainerWidget * parent):
    WContainerWidget(parent), session(sess)
{
    txtLogin = NULL;
    txtEmail = NULL;
    btnRegister = NULL;
    chRules = NULL;

    CreateRegisterPage();
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
    console(DEBUG_CODE, "RegisterPage::refresh()");
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
        addWidget(new WBreak());

    regInfo = new WText("");
    addWidget(regInfo);
    addWidget(new WBreak());
    addWidget(new WBreak());

    txtLogin = new WLineEdit();
    txtLogin->setEmptyText(Wt::WString::tr(TXT_ACC_LOGIN));
    WRegExpValidator * validator = new WRegExpValidator(LOGIN_VALIDATOR);
    validator->setMandatory(true);
    txtLogin->setValidator(validator);

    addWidget(new WText(Wt::WString::tr(TXT_ACC_LOGIN)));
    addWidget(txtLogin);
    addWidget(new WBreak());

    txtEmail = new WLineEdit();
    txtEmail->setEmptyText(Wt::WString::tr(TXT_ACC_MAIL));
    validator = new WRegExpValidator("[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,4}");
    validator->setMandatory(true);
    txtEmail->setValidator(validator);

    addWidget(new WText(Wt::WString::tr(TXT_ACC_MAIL)));
    addWidget(txtEmail);
    addWidget(new WBreak());

    chRules = new WCheckBox(Wt::WString::tr(TXT_REG_RULES));
    addWidget(chRules);
    addWidget(new WBreak());
    addWidget(new WBreak());

    btnRegister = new WPushButton(tr(TXT_BTN_REGISTER));

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
    bool validLogin = txtLogin->validate() == WValidator::Valid;
    bool validEmail = txtEmail->validate() == WValidator::Valid;
    if (!validLogin || !validEmail)
    {
        Log(LOG_INVALID_DATA, "User trying to register with invalid data ! IP: %s login: %s email: %s", session->sessionIp.toUTF8().c_str(), txtLogin->text().toUTF8().c_str(), txtEmail->text().toUTF8().c_str());
        regInfo->setText(Wt::WString::tr(TXT_ERROR_VALIDATION_LOGIN));
        return;
    }

    if (!chRules->isChecked())
    {
        regInfo->setText(Wt::WString::tr(TXT_REG_RULES_NOT_ACCEPTED));
        return;
    }

    Database db;

    if (!db.Connect(SERVER_DB_DATA, SQL_REALMDB))
    {
        regInfo->setText(Wt::WString::tr(TXT_ERROR_DB_CANT_CONNECT));
        ClearRegisterData();
        return;
    }

    WString login, mail, pass;

    login = db.EscapeString(txtLogin->text());

    // check if account already exists
    db.SetPQuery("SELECT id FROM account WHERE username = '%s'", login.toUTF8().c_str());
    if (db.ExecuteQuery() > DB_RESULT_EMPTY)
    {
        ClearRegisterData();
        regInfo->setText(Wt::WString::tr(TXT_REG_ACC_EXISTS));
        chRules->setChecked(false);
        CheckChange();
        return;
    }

    mail = txtEmail->text();

    pass = "";

    int passLen = irand(PASSWORD_LENGTH_MIN, PASSWORD_LENGTH_MAX);

    std::string tmpStr;

    for (int i = 0; i < passLen; ++i)
        tmpStr += (char)(irand(PASSWORD_ASCII_START, PASSWORD_ASCII_END));

    pass = db.EscapeString(WString::fromUTF8(tmpStr));

    WString from, msg;
    from = MAIL_FROM;

    db.SetPQuery("INSERT INTO account (username, email, sha_pass_hash, expansion) VALUES (UPPER('%s'), UPPER('%s'), SHA1(UPPER('%s:%s')), '%i')", login.toUTF8().c_str(), mail.toUTF8().c_str(), login.toUTF8().c_str(), pass.toUTF8().c_str(), STARTING_EXPANSION);

    if (db.ExecuteQuery() == DB_RESULT_ERROR)
    {
        regInfo->setText(Wt::WString::tr(TXT_REG_ERROR));
        chRules->setChecked(false);
        CheckChange();
        ClearRegisterData();
        return;
    }

    msg = tr(TXT_REG_MAIL).arg(login.toUTF8()).arg(tmpStr);

    SendMail(from, mail, Wt::WString::tr(TXT_REG_SUBJECT), msg);

    ClearRegisterData();
    chRules->setChecked(false);
    CheckChange();

    regInfo->setText(Wt::WString::tr(TXT_REG_COMPLETE));

    uint32 accId;

    if (db.ExecutePQuery("SELECT id FROM account WHERE username = '%s'", login.toUTF8().c_str()) > DB_RESULT_EMPTY)
        accId = db.GetRow()->fields[0].GetUInt32();
    else
        return;

    if (db.Connect(PANEL_DB_DATA, SQL_PANELDB))
        db.ExecutePQuery("INSERT INTO Activity VALUES ('%u', NOW(), '%s', '%s', '')", accId, session->sessionIp.toUTF8().c_str(), TXT_ACT_REGISTRATION_COMPLETE);
}

/********************************************//**
 * \}
 * \}
 *
 ***********************************************/
