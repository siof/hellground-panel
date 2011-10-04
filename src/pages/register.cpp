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
 * \addtogroup Registration
 * \{
 *
 * \file register.cpp
 * This file contains code needed to handle player registration.
 *
 ***********************************************/

#include "register.h"
#include "../database.h"

RegisterPage::RegisterPage(SessionInfo * sess, WContainerWidget * parent):
    WContainerWidget(parent), session(sess), needCreation(true)
{
    setContentAlignment(AlignCenter|AlignTop);
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
    #ifdef DEBUG
    printf("RegisterPage::refresh()");
    #endif

    if (needCreation)
        CreateRegisterPage();
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
    addWidget(textSlots[REG_TEXT_LOGIN].GetLabel());
    addWidget(txtLogin);
    addWidget(new WBreak());

    textSlots[REG_TEXT_EMAIL].SetLabel(session, TXT_LBL_ACC_MAIL);
    txtEmail = new WLineEdit();
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
    btnRegister->setEnabled(false);
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
    if (!chRules->isChecked())
    {
        textSlots[REG_TEXT_INFO].SetLabel(session, TXT_LBL_REG_RULES_NOT_ACCEPTED);
        return;
    }

    Database * db = new Database(SERVER_DB_DATA, SQL_REALMDB);
    WString login, mail, pass;

    login = db->EscapeString(txtLogin->text());

    // check if account already exists
    db->SetPQuery("SELECT id FROM account WHERE username = '%s'", login.toUTF8().c_str());
    if (db->ExecuteQuery())
    {
        ClearLogin();
        textSlots[REG_TEXT_INFO].SetLabel(session, TXT_LBL_REG_ACC_EXISTS);
        return;
    }

    mail = txtEmail->text();

    pass = "";

    int passLen = irand(PASSWORD_LENGTH_MIN, PASSWORD_LENGTH_MAX);

    std::string tmpStr;

    for (int i = 0; i < passLen; ++i)
        tmpStr += (char)(irand(PASSWORD_ASCII_START, PASSWORD_ASCII_END));

    pass = db->EscapeString(WString::fromUTF8(tmpStr));

    WString from, msg;
    from = MAIL_FROM;

    // check should be moved to other place but here will be usefull for SendMail tests ;)
    #ifdef REGISTRATION_ENABLED
    db->SetPQuery("INSERT INTO account (username, email, sha_pass_hash) VALUES ('%s', '%s', SHA1(UPPER('%s:%s')))", login.toUTF8().c_str(), mail.toUTF8().c_str(), login.toUTF8().c_str(), pass.toUTF8().c_str());
    db->ExecuteQuery();
    #endif

    if (session->HasText(TXT_REGISTRATION_MAIL))
        msg = session->GetText(TXT_REGISTRATION_MAIL);
    else
        msg = "Registration Mail\n\n Your password for login %s is: %s";

    // set buffer size (format string size + 16 chars for login + 16 chars for pass + some to be safe)
    char * buffer = new char[msg.toUTF8().size() + 40];

    sprintf(buffer, msg.toUTF8().c_str(), login.toUTF8().c_str(), tmpStr.c_str());

    msg = buffer;

    SendMail(from, mail, session->GetText(TXT_REGISTRATION_SUBJECT), msg);

    delete [] buffer;
    delete db;

    ClearLogin();
    ClearEmail();
    chRules->setChecked(false);
    CheckChange();
}

/********************************************//**
 * \}
 * \}
 *
 ***********************************************/
