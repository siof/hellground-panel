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
#include <Wt/WPushButton>
#include <Wt/WRegExpValidator>
#include <Wt/WTemplate>

#include "config.h"
#include "database.h"
#include "misc.h"
#include "miscAccount.h"
#include "miscError.h"
#include "miscHash.h"

LoginWidget::LoginWidget(SessionInfo * sess, Wt::WTemplate * tmplt, Wt::WContainerWidget * parent)
: Wt::WContainerWidget(parent)
{
    session = sess;
    templ = tmplt;

    Wt::WValidator * validator;

    login = new Wt::WLineEdit(this);
    login->setEchoMode(WLineEdit::Normal);
    login->setEmptyText(Wt::WString::tr(TXT_ACC_LOGIN));
    validator = new Wt::WRegExpValidator(sConfig.GetConfig(CONFIG_LOGIN_VALIDATOR));
    validator->setMandatory(true);
    login->setValidator(validator);

    pass = new Wt::WLineEdit(this);
    pass->setEchoMode(Wt::WLineEdit::Password);
    pass->setEmptyText(Wt::WString("pass"));
    validator = new Wt::WLengthValidator(sConfig.GetConfig(CONFIG_PASSWORD_LENGTH_MIN), sConfig.GetConfig(CONFIG_PASSWORD_LENGTH_MAX));
    validator->setMandatory(true);
    pass->setValidator(validator);

    btn = new Wt::WPushButton(Wt::WString::tr(TXT_BTN_LOGIN));
    btn->clicked().connect(this, &LoginWidget::Login);

    addWidget(login);
    addWidget(new Wt::WBreak());
    addWidget(pass);
    addWidget(new Wt::WBreak());
    addWidget(btn);

    setStyleClass("loginwidget");
}

LoginWidget::~LoginWidget()
{
    session = NULL;
}

void LoginWidget::Login()
{
    bool validLogin = login->validate() == Wt::WValidator::Valid;
    bool validPass = pass->validate() == Wt::WValidator::Valid;

    if (!validLogin || !validPass)
    {
        Misc::Log(LOG_INVALID_DATA, "User trying to log in with invalid data ! ip: %s login: %s pass: %s", session->sessionIp.toUTF8().c_str(), login->text().toUTF8().c_str(), pass->text().toUTF8().c_str());
        Misc::Error::ShowErrorBoxTr(TXT_GEN_ERROR, TXT_ERROR_VALIDATION_LOGIN);
        return;
    }

    Database db;
    if (!db.Connect(sConfig.GetConfig(CONFIG_DB_ACCOUNTS_HOST), sConfig.GetConfig(CONFIG_DB_ACCOUNTS_LOGIN),
                    sConfig.GetConfig(CONFIG_DB_ACCOUNTS_PASSWORD), sConfig.GetConfig(CONFIG_DB_ACCOUNTS_PORT), sConfig.GetConfig(CONFIG_DB_ACCOUNTS_NAME)))
    {
        Misc::Error::ShowErrorBoxTr(TXT_GEN_ERROR, TXT_ERROR_DB_CANT_CONNECT);
        return;
    }

    std::string escapedLogin = db.EscapeString(login->text());
    std::string escapedPass = db.EscapeString(pass->text());
    WString shapass = Misc::Hash::PWGetSHA1("%s:%s", Misc::Hash::HASH_FLAG_UPPER, escapedLogin.c_str(), escapedPass.c_str());

               //           0            1         2     3       4       5         6       7         8       9
    db.SetPQuery("SELECT username, sha_pass_hash, id, gmlevel, email, joindate, last_ip, locked, expansion, account_flags "
                 "FROM account "
                 "WHERE username = '%s'", escapedLogin.c_str());

    // execute will return 0 if result will be empty and -1 if there will be DB error.
    switch (db.ExecuteQuery())
    {
        case DB_RESULT_ERROR:
        {
            Misc::Account::AddActivity(escapedLogin, session->sessionIp.toUTF8(), TXT_ACT_LOGIN_FAIL, "");
            Misc::Error::ShowErrorBoxTr(TXT_GEN_ERROR, TXT_ERROR_DB_QUERY_ERROR);
            return;
        }
        case DB_RESULT_EMPTY:
        {
            Misc::Account::AddActivity(escapedLogin, session->sessionIp.toUTF8(), TXT_ACT_LOGIN_FAIL, "");
            Misc::Error::ShowErrorBoxTr(TXT_GEN_ERROR, TXT_ERROR_WRONG_LOGIN_DATA);
            return;
        }
        default:
        {
            DatabaseRow * row = db.GetRow();

            if (!row)
            {
                Misc::Error::ShowErrorBoxTr(TXT_GEN_ERROR, TXT_ERROR_WRONG_LOGIN_DATA);
                return;
            }

            if (row->fields[1].GetWString() != shapass)
            {
                Misc::Account::AddActivity(row->fields[2].GetUInt32(), session->sessionIp.toUTF8(), TXT_ACT_LOGIN_FAIL, "");
                Misc::Error::ShowErrorBoxTr(TXT_GEN_ERROR, TXT_ERROR_WRONG_LOGIN_DATA);
                //printf("\npassess: db: %s | calculated: %s\n", row->fields[1].GetCString(), shapass.toUTF8().c_str());
                return;
            }

            if (row->fields[7].GetBool() && row->fields[6].GetWString() != session->sessionIp)
            {
                Misc::Account::AddActivity(row->fields[2].GetUInt32(), session->sessionIp.toUTF8(), TXT_ACT_LOGIN_FAIL, "");
                Misc::Error::ShowErrorBoxTr(TXT_GEN_ERROR, TXT_ERROR_IP_MISMATCH);
                return;
            }

            Misc::Account::AddActivity(row->fields[2].GetUInt32(), session->sessionIp.toUTF8(), TXT_ACT_LOGIN_SUCCESS, "");

            session->login = row->fields[0].GetWString();
            session->pass = row->fields[1].GetWString();
            session->accid = row->fields[2].GetUInt64();
            session->accLvl = row->fields[3].GetAccountLevel();
            session->email = row->fields[4].GetWString();
            session->joinDate = row->fields[5].GetWString();
            session->lastIp = row->fields[6].GetWString();
            session->locked = row->fields[7].GetBool();
            session->expansion = row->fields[8].GetInt();
            session->account_flags = row->fields[9].GetUInt64();
            session->vote = 0;//row->fields[10].GetUInt32();

            if (db.ExecutePQuery("SELECT * FROM account_banned WHERE id = '%u' AND active = 1 AND (bandate = unbandate OR unbandate > UNIX_TIMESTAMP())", session->accid) > DB_RESULT_EMPTY)
                session->banned = true;

            login->setText("");
            pass->setText("");

            templ->setCondition("if-loggedin", true);
            templ->setCondition("if-notlogged", false);
            templ->refresh();
            break;
        }
    }
}
