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
 * \addtogroup Password change
 * \{
 *
 * \file passChange.cpp
 * This file contains code needed to handle account password changing.
 *
 ***********************************************/

#include "passChange.h"
#include "../database.h"

PassChangePage::PassChangePage(SessionInfo * sess, WContainerWidget * parent):
    WContainerWidget(parent), session(sess), needCreation(true)
{
    setContentAlignment(AlignCenter|AlignTop);
}

PassChangePage::~PassChangePage()
{

}

/********************************************//**
 * \brief Overloads WContainerWidget::refresh() for automatic content change.
 *
 * This function can create new or update existing content.
 * In most situations this is used for content update ;)
 *
 ***********************************************/

void PassChangePage::refresh()
{
    #ifdef DEBUG
    printf("PassChangePage::refresh()");
    #endif

    if (needCreation)
        CreatePassChangePage();
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

void PassChangePage::UpdateTextWidgets()
{
    for (int i = 0; i < PASS_CHANGE_TEXT_SLOT_COUNT; ++i)
        textSlots[i].UpdateLabel(session);

    btnChange->setText(session->GetText(TXT_BTN_PASS_CHANGE));
}

/********************************************//**
 * \brief Create change password page and it's widgets.
 *
 * Create widgets for each slot (and additional not sloted widgets) and fills labels with text.
 * This should be done only once for player.
 *
 ***********************************************/

void PassChangePage::CreatePassChangePage()
{
    clear();
    needCreation = false;

    textSlots[PASS_CHANGE_TEXT_MAIN].SetLabel(session, TXT_LBL_PASS_CHANGE);
    addWidget(textSlots[PASS_CHANGE_TEXT_MAIN].GetLabel());
    addWidget(new WBreak());
    addWidget(new WBreak());
    addWidget(new WBreak());
    addWidget(new WBreak());

    textSlots[PASS_CHANGE_TEXT_INFO].SetLabel("");
    addWidget(textSlots[PASS_CHANGE_TEXT_INFO].GetLabel());
    addWidget(new WBreak());
    addWidget(new WBreak());

    textSlots[PASS_CHANGE_TEXT_PASS].SetLabel(session, TXT_LBL_PASS_NEW);
    txtPass = new WLineEdit();
    txtPass->setEchoMode(WLineEdit::Password);
    addWidget(textSlots[PASS_CHANGE_TEXT_PASS].GetLabel());
    addWidget(txtPass);
    addWidget(new WBreak());

    textSlots[PASS_CHANGE_TEXT_PASS2].SetLabel(session, TXT_LBL_PASS_REPEAT);
    txtPass2 = new WLineEdit();
    txtPass2->setEchoMode(WLineEdit::Password);
    addWidget(textSlots[PASS_CHANGE_TEXT_PASS2].GetLabel());
    addWidget(txtPass2);
    addWidget(new WBreak());
    addWidget(new WBreak());

    btnChange = new WPushButton(session->GetText(TXT_BTN_PASS_CHANGE));
    addWidget(btnChange);

    txtPass->focussed().connect(this, &PassChangePage::ClearPass);
    txtPass2->focussed().connect(this, &PassChangePage::ClearPass2);
    btnChange->clicked().connect(this, &PassChangePage::Change);
}

/********************************************//**
 * \brief Clear passwords.
 *
 *  Clears passwords text boxes.
 *
 ***********************************************/

void PassChangePage::ClearPass()
{
    txtPass->setText(WString(""));
}

/********************************************//**
 * \brief Clear passwords.
 *
 *  Clears passwords text boxes.
 *
 ***********************************************/

void PassChangePage::ClearPass2()
{
    txtPass2->setText(WString(""));
}

/********************************************//**
 * \brief Change account password
 *
 * Function to validate data and change account password to new one.
 *
 ***********************************************/

void PassChangePage::Change()
{
    WString pass, pass2;

    pass = txtPass->text();
    pass2 = txtPass2->text();

    std::string tmpPass = pass.toUTF8();

    if (tmpPass.size() > PASSWORD_LENGTH_MAX)
    {
        textSlots[PASS_CHANGE_TEXT_INFO].SetLabel(session, TXT_ERROR_PASSWORD_TO_LONG);
        ClearPass();
        ClearPass2();
        return;
    }

    if (tmpPass.size() < PASSWORD_LENGTH_MIN)
    {
        textSlots[PASS_CHANGE_TEXT_INFO].SetLabel(session, TXT_ERROR_PASSWORD_TO_SHORT);
        ClearPass();
        ClearPass2();
        return;
    }

    if (pass != pass2)
    {
        textSlots[PASS_CHANGE_TEXT_INFO].SetLabel(session, TXT_ERROR_PASSWORDS_MISMATCH);
        ClearPass();
        ClearPass2();
        return;
    }

    std::string tmpLogin;
    Database * db = new Database(SERVER_DB_DATA, SQL_REALMDB);
    tmpPass = db->EscapeString(tmpPass);
    tmpLogin = db->EscapeString(session->login);

    // check should be moved to other place but here will be usefull for SendMail tests ;)
    db->SetPQuery("UPDATE account SET sha_pass_hash = SHA1(UPPER('%s:%s')) WHERE id = %i;", tmpLogin.c_str(), tmpPass.c_str(), session->accid);
    db->ExecuteQuery();

    delete db;

    ClearPass();
    ClearPass2();

    textSlots[PASS_CHANGE_TEXT_INFO].SetLabel(session, TXT_PASS_CHANGE_COMPLETE);
}

/********************************************//**
 * \}
 * \}
 *
 ***********************************************/
