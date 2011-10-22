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
#include "../database.h"
#include <WRegExpValidator>

RegisterPage::RegisterPage(SessionInfo * sess, WContainerWidget * parent):
    WContainerWidget(parent), session(sess), needCreation(true)
{
    setContentAlignment(AlignCenter|AlignTop);

    txtLogin = NULL;
    txtEmail = NULL;
    btnRegister = NULL;
    chRules = NULL;
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

    // registration page should be only for not logged yet players so there is no need to update it in other cases
    if (session->accLvl == LVL_NOT_LOGGED)
    {
        if (needCreation)
            CreateRegisterPage();
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

void RegisterPage::UpdateTextWidgets()
{
    for (int i = 0; i < REG_TEXT_SLOT_COUNT; ++i)
        textSlots[i].UpdateLabel(session);

    chRules->setText(session->GetText(TXT_LBL_REG_RULES_ACCEPT));
    btnRegister->setText(session->GetText(TXT_BTN_REGISTER));
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
    clear();
    needCreation = false;

    textSlots[REG_TEXT_MAIN].SetLabel(session, TXT_LBL_REG_MAIN);
    addWidget(textSlots[REG_TEXT_MAIN].GetLabel());
    addWidget(new WBreak());
    addWidget(new WBreak());
    addWidget(new WBreak());
    addWidget(new WBreak());

    textSlots[REG_TEXT_INFO].SetLabel("");
    addWidget(textSlots[REG_TEXT_INFO].GetLabel());
    addWidget(new WBreak());
    addWidget(new WBreak());

    textSlots[REG_TEXT_LOGIN].SetLabel(session, TXT_LBL_ACC_LOGIN);
    txtLogin = new WLineEdit();
    WRegExpValidator * validator = new WRegExpValidator(LOGIN_VALIDATOR);
    txtLogin->setValidator(validator);

    addWidget(textSlots[REG_TEXT_LOGIN].GetLabel());
    addWidget(txtLogin);
    addWidget(new WBreak());

    textSlots[REG_TEXT_EMAIL].SetLabel(session, TXT_LBL_ACC_MAIL);
    txtEmail = new WLineEdit();
    validator = new WRegExpValidator("[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,4}");
    txtEmail->setValidator(validator);

    addWidget(textSlots[REG_TEXT_EMAIL].GetLabel());
    addWidget(txtEmail);
    addWidget(new WBreak());

    textSlots[REG_TEXT_RULES].SetLabel(session, TXT_LBL_REG_RULES);
    addWidget(textSlots[REG_TEXT_RULES].GetLabel());
    addWidget(new WBreak());

//    textSlots[REG_TEXT_RULES_ACCEPT].SetLabel(session, TXT_LBL_REGISTER_RULES_ACCEPT);
    chRules = new WCheckBox(session->GetText(TXT_LBL_REG_RULES_ACCEPT));
//    addWidget(textSlots[REG_TEXT_RULES_ACCEPT].GetLabel());
    addWidget(chRules);
    addWidget(new WBreak());
    addWidget(new WBreak());

    btnRegister = new WPushButton(session->GetText(TXT_BTN_REGISTER));

    if (wApp->environment().javaScript())
        btnRegister->setDisabled(true);

    addWidget(btnRegister);

    txtLogin->focussed().connect(this, &RegisterPage::ClearLogin);
    txtEmail->focussed().connect(this, &RegisterPage::ClearEmail);
    chRules->changed().connect(this, &RegisterPage::CheckChange);
    btnRegister->clicked().connect(this, &RegisterPage::Register);
}

/********************************************//**
 * \brief Clear login.
 *
 *  Clears login text box.
 *
 ***********************************************/

void RegisterPage::ClearLogin()
{
    txtLogin->setText(WString(""));
}

/********************************************//**
 * \brief Clear email.
 *
 * Clears email text box.
 *
 ***********************************************/

void RegisterPage::ClearEmail()
{
    txtEmail->setText(WString(""));
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
        textSlots[REG_TEXT_INFO].SetLabel(session, TXT_ERROR_NOT_VALID_DATA);
        return;
    }

    if (!chRules->isChecked())
    {
        textSlots[REG_TEXT_INFO].SetLabel(session, TXT_LBL_REG_RULES_NOT_ACCEPTED);
        return;
    }

    Database db;

    if (!db.Connect(SERVER_DB_DATA, SQL_REALMDB))
    {
        textSlots[REG_TEXT_INFO].SetLabel(session, TXT_DBERROR_CANT_CONNECT);
        return;
    }

    WString login, mail, pass;

    login = db.EscapeString(txtLogin->text());

    // check if account already exists
    db.SetPQuery("SELECT id FROM account WHERE username = '%s'", login.toUTF8().c_str());
    if (db.ExecuteQuery() > RETURN_EMPTY)
    {
        ClearLogin();
        textSlots[REG_TEXT_INFO].SetLabel(session, TXT_LBL_REG_ACC_EXISTS);
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

    db.SetPQuery("INSERT INTO account (username, email, sha_pass_hash) VALUES ('%s', '%s', SHA1(UPPER('%s:%s')))", login.toUTF8().c_str(), mail.toUTF8().c_str(), login.toUTF8().c_str(), pass.toUTF8().c_str());

    if (db.ExecuteQuery() == RETURN_ERROR)
    {
        textSlots[REG_TEXT_INFO].SetLabel(session, TXT_REGISTRATION_ERROR);
        chRules->setChecked(false);
        CheckChange();
        return;
    }

    if (session->HasText(TXT_REGISTRATION_MAIL))
        msg = GetFormattedString(session->GetText(TXT_REGISTRATION_MAIL).toUTF8().c_str(), login.toUTF8().c_str(), tmpStr.c_str());
    else
        msg = GetFormattedString("Registration Mail\n\n Your password for login %s is: %s", login.toUTF8().c_str(), tmpStr.c_str());

    SendMail(from, mail, session->GetText(TXT_REGISTRATION_SUBJECT), msg);

    ClearLogin();
    ClearEmail();
    chRules->setChecked(false);
    CheckChange();

    textSlots[REG_TEXT_INFO].SetLabel(session, TXT_REGISTRATION_COMPLETE);

    uint32 accId;

    if (db.ExecutePQuery("SELECT id FROM account WHERE username = '%s'", login.toUTF8().c_str()) > RETURN_EMPTY)
        accId = db.GetRow()->fields[0].GetUInt32();
    else
        return;

    if (db.Connect(PANEL_DB_DATA, SQL_PANELDB))
        db.ExecutePQuery("INSERT INTO Activity VALUES ('XXX', '%u', NOW(), '%s', '%u', '')", accId, session->sessionIp.toUTF8().c_str(), TXT_ACT_REGISTRATION_COMPLETE);
}

/********************************************//**
 * \}
 * \}
 *
 ***********************************************/
