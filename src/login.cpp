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

    password = new Wt::WLineEdit(this);
    password->setEchoMode(Wt::WLineEdit::Password);
    password->setEmptyText(Wt::WString("pass"));
    validator = new Wt::WLengthValidator(sConfig.GetConfig(CONFIG_PASSWORD_LENGTH_MIN), sConfig.GetConfig(CONFIG_PASSWORD_LENGTH_MAX));
    validator->setMandatory(true);
    password->setValidator(validator);

    btn = new Wt::WPushButton(Wt::WString::tr(TXT_BTN_LOGIN));
    btn->clicked().connect(this, &LoginWidget::Login);

    addWidget(login);
    addWidget(new Wt::WBreak());
    addWidget(password);
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
    bool validPass = password->validate() == Wt::WValidator::Valid;

    if (!validLogin || !validPass)
    {
        Misc::Log(LOG_INVALID_DATA, "User trying to log in with invalid data ! ip: %s login: %s pass: %s", session->sessionIp.toUTF8().c_str(), login->text().toUTF8().c_str(), password->text().toUTF8().c_str());
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
    std::string escapedPass = db.EscapeString(password->text());
    WString shapass = Misc::Hash::PWGetSHA1("%s:%s", Misc::Hash::HASH_FLAG_UPPER, escapedLogin.c_str(), escapedPass.c_str());

               //           0          1           2          3        4         5            6              7           8               9               10
    db.SetPQuery("SELECT username, pass_hash, a.account_id, email, join_date, last_ip, account_state_id, expansion, account_flags, support_points, permission_mask "
                 "FROM account AS a JOIN account_support AS as ON a.account_id = as.account_id JOIN account_permissions AS ap ON a.account_id = ap.account_id "
                 "WHERE username = '%s' AND realm_id = '%i'", escapedLogin.c_str(), session->currentRealm);

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

            uint8 accountState = row->fields[6].GetInt();

            switch (accountState)
            {
                case ACCOUNT_STATE_INACTIVE:
                {
                    Misc::Account::AddActivity(row->fields[2].GetUInt32(), session->sessionIp.toUTF8(), TXT_ACT_LOGIN_FAIL, "");
                    Misc::Error::ShowErrorBoxTr(TXT_GEN_ERROR, TXT_ERROR_ACCOUNT_INACTIVE);
                    return;
                }
                case ACCOUNT_STATE_IP_LOCKED:
                {
                    if (row->fields[5].GetWString() != session->sessionIp)
                    {
                        Misc::Account::AddActivity(row->fields[2].GetUInt32(), session->sessionIp.toUTF8(), TXT_ACT_LOGIN_FAIL, "");
                        Misc::Error::ShowErrorBoxTr(TXT_GEN_ERROR, TXT_ERROR_IP_MISMATCH);
                        return;
                    }
                    break;
                }
                case ACCOUNT_STATE_ACTIVE:
                case ACCOUNT_STATE_FROZEN:
                    break;
                default:
                {
                    Misc::Account::AddActivity(row->fields[2].GetUInt32(), session->sessionIp.toUTF8(), TXT_ACT_LOGIN_FAIL, "");
                    Misc::Error::ShowErrorBoxTr(TXT_GEN_ERROR, TXT_ERROR_ACCOUNT_STATE_UNKNOWN);
                    return;
                }
            }

            Misc::Account::AddActivity(row->fields[2].GetUInt32(), session->sessionIp.toUTF8(), TXT_ACT_LOGIN_SUCCESS, "");

            session->login = row->fields[0].GetWString();
            session->password = row->fields[1].GetWString();
            session->accountId = row->fields[2].GetUInt64();
            //session->accLvl = row->fields[3].GetAccountLevel();
            session->email = row->fields[3].GetWString();
            session->joinDate = row->fields[4].GetWString();
            session->lastIp = row->fields[5].GetWString();
            session->accountState = static_cast<AccountState>(accountState);
            session->expansion = row->fields[7].GetInt();
            session->accountFlags = row->fields[8].GetUInt64();
            session->supportPoints = row->fields[9].GetUInt32();
            session->permissions = row->fields[10].GetUInt64();

            if (db.ExecutePQuery("SELECT 1 FROM account_punishment WHERE account_id = '%u' AND punishment_type_id = '%u' AND (punishment_date = expiration_date OR expiration_date > UNIX_TIMESTAMP())", session->accountId) > DB_RESULT_EMPTY)
                session->banned = true;

            login->setText("");
            password->setText("");

            templ->setCondition("if-loggedin", true);
            templ->setCondition("if-notlogged", false);
            templ->refresh();
            break;
        }
    }
}
