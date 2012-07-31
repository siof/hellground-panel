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
 * \addtogroup Passwords Passwords Management
 * \{
 *
 * \file passChange.cpp
 * This file contains code needed to handle account password changing.
 *
 ***********************************************/

#include "passChange.h"

#include <Wt/WBreak>
#include <Wt/WLineEdit>
#include <Wt/WPushButton>
#include <Wt/WText>

#include "../database.h"
#include "../misc.h"
#include "../miscHash.h"

PassChangePage::PassChangePage(SessionInfo * sess, WContainerWidget * parent):
    WContainerWidget(parent), session(sess)
{
    txtPass = NULL;
    txtPass2 = NULL;
    btnChange = NULL;

    CreatePassChangePage();
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
    Misc::Console(DEBUG_CODE, "PassChangePage::refresh()");
    Wt::WContainerWidget::refresh();
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
    addWidget(new Wt::WText(Wt::WString::tr(TXT_INFO_PASS_CHANGE)));
    for (int i = 0; i < 4; ++i)
        addWidget(new Wt::WBreak());

    changeInfo = new Wt::WText("");
    addWidget(changeInfo);
    addWidget(new Wt::WBreak());
    addWidget(new Wt::WBreak());

    txtPassOld = new Wt::WLineEdit();
    txtPassOld->setEmptyText(Wt::WString::tr(TXT_PASS_OLD));
    txtPassOld->setEchoMode(Wt::WLineEdit::Password);
    addWidget(new Wt::WText(Wt::WString::tr(TXT_PASS_OLD)));
    addWidget(txtPassOld);
    addWidget(new Wt::WBreak());

    txtPass = new Wt::WLineEdit();
    txtPass->setEmptyText(Wt::WString::tr(TXT_PASS_NEW));
    txtPass->setEchoMode(WLineEdit::Password);
    addWidget(new Wt::WText(Wt::WString::tr(TXT_PASS_NEW)));
    addWidget(txtPass);
    addWidget(new Wt::WBreak());

    txtPass2 = new Wt::WLineEdit();
    txtPass2->setEmptyText(Wt::WString::tr(TXT_PASS_REPEAT));
    txtPass2->setEchoMode(Wt::WLineEdit::Password);
    addWidget(new Wt::WText(Wt::WString::tr(TXT_PASS_REPEAT)));
    addWidget(txtPass2);
    addWidget(new Wt::WBreak());
    addWidget(new Wt::WBreak());

    btnChange = new Wt::WPushButton(Wt::WString::tr(TXT_BTN_PASS_CHANGE));
    addWidget(btnChange);

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

    Wt::WString pass, pass2;

    pass = txtPass->text();
    pass2 = txtPass2->text();

    std::string tmpPass = pass.toUTF8();

    if (tmpPass.size() > PASSWORD_LENGTH_MAX)
    {
        changeInfo->setText(Wt::WString::tr(TXT_ERROR_PASSWORD_TO_LONG));
        ClearPass();
        return;
    }

    if (tmpPass.size() < PASSWORD_LENGTH_MIN)
    {
        changeInfo->setText(Wt::WString::tr(TXT_ERROR_PASSWORD_TO_SHORT));
        ClearPass();
        return;
    }

    if (pass != pass2)
    {
        changeInfo->setText(Wt::WString::tr(TXT_ERROR_PASSWORDS_MISMATCH));
        ClearPass();
        return;
    }

    Database db;

    if (db.Connect(PANEL_DB_DATA, SQL_PANELDB))
        db.ExecutePQuery("INSERT INTO Activity VALUES ('%u', NOW(), '%s', '%s', '')", session->accid, session->sessionIp.toUTF8().c_str(), TXT_ACT_PASS_CHANGE);

    Wt::WString shapass;
    std::string escapedLogin = db.EscapeString(session->login);
    std::string escapedPass = db.EscapeString(txtPassOld->text());
    shapass = Misc::Hash::PWGetSHA1("%s:%s", Misc::Hash::HASH_FLAG_UPPER, escapedLogin.c_str(), escapedPass.c_str());

    if (shapass != session->pass)
    {
        Misc::Console(DEBUG_CODE, "void PassChangePage::Change(): oldPass: %s , shapass: %s , pass: %s\n", txtPassOld->text().toUTF8().c_str(), shapass.toUTF8().c_str(), session->pass.toUTF8().c_str());
        changeInfo->setText(Wt::WString::tr(TXT_ERROR_WRONG_PASSWORD));
        ClearPass();
        return;
    }

    escapedPass = db.EscapeString(pass);
    shapass = Misc::Hash::PWGetSHA1("%s:%s", Misc::Hash::HASH_FLAG_UPPER, escapedLogin.c_str(), escapedPass.c_str());

    if (!db.Connect(SERVER_DB_DATA, SQL_REALMDB))
    {
        changeInfo->setText(Wt::WString::tr(TXT_ERROR_DB_CANT_CONNECT));
        return;
    }

    db.SetPQuery("UPDATE account SET sha_pass_hash = '%s', sessionkey = NULL, s = NULL, v = NULL WHERE id = '%u'", shapass.toUTF8().c_str(), session->accid);

    if (db.ExecuteQuery() == DB_RESULT_ERROR)
        changeInfo->setText(Wt::WString::tr(TXT_ERROR_DB_QUERY_ERROR));
    else
    {
        session->pass = shapass;
        changeInfo->setText(Wt::WString::tr(TXT_PASS_CHANGE_COMPLETE));
    }

    ClearPass();
}

/********************************************//**
 * \}
 * \}
 *
 ***********************************************/
