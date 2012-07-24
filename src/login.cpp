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

#include "login.h"

#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WLengthValidator>
#include <Wt/WLineEdit>
#include <Wt/WMessageBox>
#include <Wt/WPushButton>
#include <Wt/WRegExpValidator>
#include <Wt/WTemplate>

#include "database.h"

LoginWidget::LoginWidget(SessionInfo * sess, Wt::WTemplate * tmplt, Wt::WContainerWidget * parent)
: Wt::WContainerWidget(parent)
{
    session = sess;
    templ = tmplt;

    Wt::WValidator * validator;

    login = new Wt::WLineEdit(this);
    login->setEchoMode(WLineEdit::Normal);
    login->setEmptyText(Wt::WString::tr(TXT_ACC_LOGIN));
    validator = new Wt::WRegExpValidator(LOGIN_VALIDATOR);
    validator->setMandatory(true);
    login->setValidator(validator);

    pass = new Wt::WLineEdit(this);
    pass->setEchoMode(Wt::WLineEdit::Password);
    pass->setEmptyText(Wt::WString("pass"));
    validator = new Wt::WLengthValidator(PASSWORD_LENGTH_MIN, PASSWORD_LENGTH_MAX);
    validator->setMandatory(true);
    pass->setValidator(validator);

    btn = new Wt::WPushButton(Wt::WString::tr(TXT_BTN_LOGIN));
    btn->clicked().connect(this, &LoginWidget::Login);

    addWidget(login);
    addWidget(new Wt::WBreak());
    addWidget(pass);
    addWidget(new Wt::WBreak());
    addWidget(btn);
}

LoginWidget::~LoginWidget()
{
    session = NULL;
}

void LoginWidget::ShowError(const char * name, const char * txt)
{
    WMessageBox::show(Wt::WString::tr(name), Wt::WString::tr(txt), Ok);
}

void LoginWidget::Login()
{
    bool validLogin = login->validate() == Wt::WValidator::Valid;
    bool validPass = pass->validate() == Wt::WValidator::Valid;

    if (!validLogin || !validPass)
    {
        Log(LOG_INVALID_DATA, "User trying to log in with invalid data ! ip: %s login: %s pass: %s", session->sessionIp.toUTF8().c_str(), login->text().toUTF8().c_str(), pass->text().toUTF8().c_str());
        ShowError(TXT_GEN_ERROR, TXT_ERROR_VALIDATION_LOGIN);
        return;
    }

    Database db;
    if (!db.Connect(SERVER_DB_DATA, SQL_REALMDB))
    {
        ShowError(TXT_GEN_ERROR, TXT_ERROR_DB_CANT_CONNECT);
        return;
    }

    std::string escapedLogin = db.EscapeString(login->text());
    std::string escapedPass = db.EscapeString(pass->text());
    std::string passStr = GetFormattedString("%s:%s", escapedLogin.c_str(), escapedPass.c_str());
    WString shapass = WGetUpperSHA1(passStr);

               //           0            1         2     3       4       5         6       7         8       9         10
    db.SetPQuery("SELECT username, sha_pass_hash, id, gmlevel, email, joindate, last_ip, locked, expansion, vote, account_flags "
                 "FROM account "
                 "WHERE username = '%s'", escapedLogin.c_str());

    // execute will return 0 if result will be empty and -1 if there will be DB error.
    switch (db.ExecuteQuery())
    {
        case DB_RESULT_ERROR:
        {
            AddActivityLogIn(false, escapedLogin.c_str());
            ShowError(TXT_GEN_ERROR, TXT_ERROR_DB_QUERY_ERROR);
            return;
        }
        case DB_RESULT_EMPTY:
        {
            AddActivityLogIn(false, escapedLogin.c_str());
            ShowError(TXT_GEN_ERROR, TXT_ERROR_WRONG_LOGIN_DATA);
            return;
        }
        default:
        {
            DatabaseRow * row = db.GetRow();

            if (!row)
            {
                ShowError(TXT_GEN_ERROR, TXT_ERROR_WRONG_LOGIN_DATA);
                return;
            }

            if (row->fields[1].GetWString() != shapass)
            {
                AddActivityLogIn(row->fields[2].GetUInt32(), false);
                ShowError(TXT_GEN_ERROR, TXT_ERROR_WRONG_LOGIN_DATA);
                return;
            }

            if (row->fields[7].GetBool() && row->fields[6].GetWString() != session->sessionIp)
            {
                AddActivityLogIn(row->fields[2].GetUInt32(), false);
                ShowError(TXT_GEN_ERROR, TXT_ERROR_IP_MISMATCH);
                return;
            }

            AddActivityLogIn(row->fields[2].GetUInt32(), true);

            session->login = row->fields[0].GetWString();
            session->pass = row->fields[1].GetWString();
            session->accid = row->fields[2].GetUInt64();
            session->accLvl = row->fields[3].GetAccountLevel();
            session->email = row->fields[4].GetWString();
            session->joinDate = row->fields[5].GetWString();
            session->lastIp = row->fields[6].GetWString();
            session->locked = row->fields[7].GetBool();
            session->expansion = row->fields[8].GetInt();
            session->vote = row->fields[9].GetUInt32();
            session->account_flags = row->fields[10].GetUInt64();

            if (db.ExecutePQuery("SELECT * FROM account_banned WHERE id = '%u' AND active = 1 AND (bandate = unbandate OR unbandate > UNIX_TIMESTAMP())", session->accid) > DB_RESULT_EMPTY)
                session->banned = true;

            login->setText("");
            pass->setText("");

            templ->setCondition("if-loggedin", true);
            templ->setCondition("if-notlogged", false);
            templ->refresh();
            //wApp->root()->refresh();
            break;
        }
    }
}

void LoginWidget::AddActivityLogIn(bool success, const char * login)
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

    AddActivityLogIn(accId, success);
}

void LoginWidget::AddActivityLogIn(uint32 id, bool success)
{
    if (!id)
        return;

    Database db;

    db.Connect(PANEL_DB_DATA, SQL_PANELDB);
    db.ExecutePQuery("INSERT INTO Activity VALUES ('%u', NOW(), '%s', '%s', '')", id, session->sessionIp.toUTF8().c_str(), success ? TXT_ACT_LOGIN_SUCCESS : TXT_ACT_LOGIN_FAIL);
}
