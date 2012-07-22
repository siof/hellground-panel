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

void LoginWidget::Login()
{
    bool validLogin = login->validate() == Wt::WValidator::Valid;
    bool validPass = pass->validate() == Wt::WValidator::Valid;

    if (!validLogin || !validPass)
    {
        Log(LOG_INVALID_DATA, "User trying to log in with invalid data ! ip: %s login: %s pass: %s", session->sessionIp.toUTF8().c_str(), login->text().toUTF8().c_str(), pass->text().toUTF8().c_str());
        return;
    }

    Database db;
    if (!db.Connect(SERVER_DB_DATA, SQL_REALMDB))
    {
//        ClearLoginData();
        return;
    }

    // simple login check ... should be replaced with something better

    std::string escapedLogin = db.EscapeString(login->text());
    std::string escapedPass = db.EscapeString(pass->text());
    WString shapass;

    switch (db.ExecutePQuery("SELECT SHA1(UPPER('%s:%s'))", escapedLogin.c_str(), escapedPass.c_str()))
    {
        case DB_RESULT_ERROR:
        {
//            AddActivityLogIn(false, escapedLogin.c_str());
            // if there was database error
//            ClearLoginData();
            return;
        }
        case DB_RESULT_EMPTY:
        {
//            AddActivityLogIn(false, escapedLogin.c_str());
            Log(LOG_STRANGE_DATA, "User with IP: %s tried to login with strange data (SHA return empty)! login: %s pass: %s", session->sessionIp.toUTF8().c_str(), escapedLogin.c_str(), escapedPass.c_str());
            //if wrong data
//            ClearLoginData();
            return;
        }
        default:
        {
            shapass = db.GetRow()->fields[0].GetWString();
            break;
        }
    }

               //           0            1         2     3       4       5         6       7         8       9         10
    db.SetPQuery("SELECT username, sha_pass_hash, id, gmlevel, email, joindate, last_ip, locked, expansion, vote, account_flags "
                 "FROM account "
                 "WHERE username = '%s'", escapedLogin.c_str());

    // execute will return 0 if result will be empty and -1 if there will be DB error.
    switch (db.ExecuteQuery())
    {
        case DB_RESULT_ERROR:
        {
//            AddActivityLogIn(false, escapedLogin.c_str());
            // if there was database error
//            ClearLoginData();
            return;
        }
        case DB_RESULT_EMPTY:
        {
//            AddActivityLogIn(false, escapedLogin.c_str());
            //if wrong data
//            ClearLoginData();
            return;
        }
        default:
        {
            DatabaseRow * row = db.GetRow();

            if (!row)
            {
//                ClearLoginData();
                return;
            }

            if (row->fields[1].GetWString() != shapass)
            {
//                AddActivityLogIn(row->fields[2].GetUInt32(), false);
                //if wrong data
//                ClearLoginData();
                return;
            }

            if (row->fields[7].GetBool() && row->fields[6].GetWString() != session->sessionIp)
            {
//                AddActivityLogIn(row->fields[2].GetUInt32(), false);
//                ClearLoginData();
                return;
            }

//            AddActivityLogIn(row->fields[2].GetUInt32(), true);

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

//            WAnimation fade(WAnimation::Fade, WAnimation::Linear, 250);
//            login->setHidden(true, fade);
//            pass->setHidden(true, fade);
//            btnLog->setHidden(true, fade);
//            login->setDisabled(true);
//            pass->setDisabled(true);
//            btnLog->setDisabled(true);
//            setHidden(true);
//            refresh();
            templ->setCondition("if-loggedin", true);
            templ->setCondition("if-notlogged", false);
            //templ->refresh();
            wApp->root()->refresh();
            break;
        }
    }

//    ClearLoginData();
}