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
 * \addtogroup Passwords Passwords Management
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
    txtPass = NULL;
    txtPass2 = NULL;
    btnChange = NULL;
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
    console(DEBUG_CODE, "PassChangePage::refresh()");

    // pass change page should be visible only for logged players so there is no need to create/update in other cases
    if (session->accLvl > LVL_NOT_LOGGED)
    {
        if (needCreation)
            CreatePassChangePage();
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

    textSlots[PASS_CHANGE_TEXT_OLDPASS].SetLabel(session, TXT_LBL_PASS_OLD);
    txtPassOld = new WLineEdit();
    txtPassOld->setEchoMode(WLineEdit::Password);
    addWidget(textSlots[PASS_CHANGE_TEXT_OLDPASS].GetLabel());
    addWidget(txtPassOld);
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

    txtPass->focussed().connect(this, &PassChangePage::ClearWLineEdit);
    txtPass2->focussed().connect(this, &PassChangePage::ClearWLineEdit);
    txtPassOld->focussed().connect(this, &PassChangePage::ClearWLineEdit);
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
    txtPass->setText("");
    txtPass2->setText("");
    txtPassOld->setText("");
}

/********************************************//**
 * \brief Change account password
 *
 * Function to validate data and change account password to new one.
 *
 ***********************************************/

void PassChangePage::Change()
{
    if (session->accLvl < LVL_PLAYER)
        return;

    WString pass, pass2, oldPass;

    oldPass = txtPassOld->text();
    pass = txtPass->text();
    pass2 = txtPass2->text();

    std::string tmpPass = pass.toUTF8();

    if (tmpPass.size() > PASSWORD_LENGTH_MAX)
    {
        textSlots[PASS_CHANGE_TEXT_INFO].SetLabel(session, TXT_ERROR_PASSWORD_TO_LONG);
        ClearPass();
        return;
    }

    if (tmpPass.size() < PASSWORD_LENGTH_MIN)
    {
        textSlots[PASS_CHANGE_TEXT_INFO].SetLabel(session, TXT_ERROR_PASSWORD_TO_SHORT);
        ClearPass();
        return;
    }

    if (pass != pass2)
    {
        textSlots[PASS_CHANGE_TEXT_INFO].SetLabel(session, TXT_ERROR_PASSWORDS_MISMATCH);
        ClearPass();
        return;
    }

    std::string tmpLogin;
    Database db;

    if (db.Connect(PANEL_DB_DATA, SQL_PANELDB))
        db.ExecutePQuery("INSERT INTO Activity VALUES ('XXX', '%u', NOW(), '%s', '%u', '')", session->accid, session->sessionIp.toUTF8().c_str(), TXT_ACT_PASS_CHANGE);

    if (oldPass != session->pass)
    {
        textSlots[PASS_CHANGE_TEXT_INFO].SetLabel(session, TXT_ERROR_WRONG_PASSWORD);
        ClearPass();
        return;
    }

    if (!db.Connect(SERVER_DB_DATA, SQL_REALMDB))
    {
        textSlots[PASS_CHANGE_TEXT_INFO].SetLabel(session, TXT_DBERROR_CANT_CONNECT);
        return;
    }

    tmpPass = db.EscapeString(tmpPass);
    tmpLogin = db.EscapeString(session->login);

    std::string sha;

    if (db.ExecutePQuery("SELECT SHA1(UPPER('%s:%s'))", tmpLogin.c_str(), tmpPass.c_str()) > RETURN_EMPTY)
        sha = db.GetRow()->fields[0].GetString();
    else
    {
        textSlots[PASS_CHANGE_TEXT_INFO].SetLabel(session, TXT_DBERROR_QUERY_ERROR);
        return;
    }

    db.SetPQuery("UPDATE account SET sha_pass_hash = '%s', sessionkey = '', s = '', v = '' WHERE id = '%u'", sha.c_str(), session->accid);

    if (db.ExecuteQuery() == RETURN_ERROR)
        textSlots[PASS_CHANGE_TEXT_INFO].SetLabel(session, TXT_DBERROR_QUERY_ERROR);
    else
        textSlots[PASS_CHANGE_TEXT_INFO].SetLabel(session, TXT_PASS_CHANGE_COMPLETE);

    ClearPass();
}

/********************************************//**
 * \}
 * \}
 *
 ***********************************************/
