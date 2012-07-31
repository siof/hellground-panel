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

#include "miscAccount.h"

#include "database.h"
#include "misc.h"

std::string Misc::Account::GeneratePassword()
{
    int passLen = Misc::Irand(PASSWORD_LENGTH_MIN, PASSWORD_LENGTH_MAX);

    std::string tmpStr;

    for (int i = 0; i < passLen; ++i)
        tmpStr += char(Misc::Irand(PASSWORD_ASCII_START, PASSWORD_ASCII_END));

    return tmpStr;
}

void Misc::Account::AddActivity(uint32 accountId, const char * ip, const char * activity, const char * activityArgs)
{
    if (!accountId || !activity || !activityArgs)
        return;

    Database db;

    db.Connect(PANEL_DB_DATA, SQL_PANELDB);
    db.ExecutePQuery("INSERT INTO Activity VALUES ('%u', NOW(), '%s', '%s', '%s')", accountId, ip, activity, activityArgs);
}

void Misc::Account::AddActivity(uint32 accountId, const std::string & ip, const char * activity, const std::string & activityArgs)
{
    if (!accountId || !activity)
        return;

    Database db;

    db.Connect(PANEL_DB_DATA, SQL_PANELDB);
    db.ExecutePQuery("INSERT INTO Activity VALUES ('%u', NOW(), '%s', '%s', '%s')", accountId, ip.c_str(), activity, activityArgs.c_str());

}

void Misc::Account::AddActivity(const char * username, const char * ip, const char * activity, const char * activityArgs, bool escape)
{
    if (!username || !activity || !activityArgs)
        return;

    Database db;

    if (!db.Connect(SERVER_DB_DATA, SQL_REALMDB))
        return;

    uint32 accountId;

    std::string tmpUsr;

    if (escape)
        tmpUsr = db.EscapeString(username);
    else
        tmpUsr = username;

    if (db.ExecutePQuery("SELECT id FROM account WHERE username = '%s'", tmpUsr.c_str()) > DB_RESULT_EMPTY)
        accountId = db.GetRow()->fields[0].GetUInt32();
    else
        return;

    db.Disconnect();

    AddActivity(accountId, ip, activity, activityArgs);
}

void Misc::Account::AddActivity(const std::string & username, const std::string & ip, const char * activity, const std::string & activityArgs, bool escape)
{
    if (username.empty() || !activity)
        return;

    Database db;

    if (!db.Connect(SERVER_DB_DATA, SQL_REALMDB))
        return;

    uint32 accountId;

    std::string tmpUsr;

    if (escape)
        tmpUsr = db.EscapeString(username);
    else
        tmpUsr = username;

    if (db.ExecutePQuery("SELECT id FROM account WHERE username = '%s'", tmpUsr.c_str()) > DB_RESULT_EMPTY)
        accountId = db.GetRow()->fields[0].GetUInt32();
    else
        return;

    db.Disconnect();

    AddActivity(accountId, ip, activity, activityArgs);
}
