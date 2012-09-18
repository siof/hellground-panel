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

#include "database.h"

#include <cstdarg>
#include <cstring>

#include "misc.h"

/// DatabaseField

Wt::WString DatabaseField::GetWString()
{
    return value;
}

const char * DatabaseField::GetCString()
{
    return value.toUTF8().c_str();
}

std::string DatabaseField::GetString()
{
    return value.toUTF8();
}

uint64 DatabaseField::GetUInt64()
{
    uint64 tmp = 0;
    sscanf(value.toUTF8().c_str(), "%lu", &tmp);
    return tmp;
}

uint32 DatabaseField::GetUInt32()
{
    uint32 tmp = 0;
    sscanf(value.toUTF8().c_str(), "%u", &tmp);
    return tmp;
}

int DatabaseField::GetInt()
{
    int tmp = 0;
    sscanf(value.toUTF8().c_str(), "%i", &tmp);
    return tmp;
}

bool DatabaseField::GetBool()
{
    int tmp = 0;
    sscanf(value.toUTF8().c_str(), "%i", &tmp);

    if (tmp)
        return true;

    return false;
}

AccountLevel DatabaseField::GetAccountLevel()
{
    int tmp = 0;
    sscanf(value.toUTF8().c_str(), "%i", &tmp);

    switch (tmp)
    {
        case LVL_PLAYER:
        case LVL_GM_TRIAL:
        case LVL_GM_HELPER:
        case LVL_GM_HEAD:
        case LVL_ADM:
            return AccountLevel(tmp);
        default:
            return LVL_PLAYER;

    }
}

/// DatabaseRow
DatabaseRow::DatabaseRow(MYSQL_ROW row, int count)
{
    Misc::Console(DEBUG_DB, "Call DatabaseRow::DatabaseRow(MYSQL_ROW row, int count = %i)\n", count);

    this->count = count;
    fields = new DatabaseField[count];
    for (int i = 0; i < count; ++i)
        fields[i].value = WString::fromUTF8(row[i] ? row[i] : "");
}

DatabaseRow::~DatabaseRow()
{
    delete [] fields;
}

/// Database

Database::Database()
{
    connection = NULL;
    loggingEnabled = true;
}

Database::~Database()
{
    Disconnect();
    Clear();
}

void Database::SetQuery(const std::string & query)
{
    actualQuery = query;
}

bool Database::SetPQuery(const char *format, ...)
{
    va_list ap;
    char szQuery[MAX_QUERY_LEN];
    va_start(ap, format);
    int res = vsnprintf(szQuery, MAX_QUERY_LEN, format, ap);
    va_end(ap);

    if (res == DB_RESULT_ERROR)
        return false;

    SetQuery(szQuery);
    return true;
}

bool Database::Connect(std::string host, std::string login, std::string pass, unsigned int port, std::string db)
{
    if (connection)
    {
        Disconnect();
        Clear();
    }

    connection = mysql_init(NULL);

    return mysql_real_connect(connection, host.c_str(), login.c_str(), pass.c_str(), db.c_str(), port, NULL, 0) != NULL;
}

void Database::Disconnect()
{
    if (connection)
        mysql_close(connection);

    connection = NULL;
}

bool Database::SelectDatabase(std::string db)
{
    return mysql_select_db(connection, db.c_str()) != NULL;
}

int Database::ExecuteQuery()
{
    Misc::Console(DEBUG_DB, "\nCall int Database::ExecuteQuery() : actualQuery: %s", actualQuery.c_str());

    Clear();

    Misc::Console(DEBUG_DB, "\n\nExecuteQuery(): test1\n");

    if (loggingEnabled)
        Misc::Log(LOG_DB_QUERY, "DB Query execute: %s", actualQuery.c_str());

    if (mysql_query(connection, actualQuery.c_str()))
    {
        if (loggingEnabled)
            Misc::Log(LOG_DB_ERRORS, "DB Query error ! Error [%i]: %s", GetErrNo(), GetError());
        return DB_RESULT_ERROR;
    }

    Misc::Console(DEBUG_DB, "\n\nExecuteQuery(): test2\n");

    MYSQL_RES * res = mysql_store_result(connection);

    if (res)
    {
        unsigned int count = mysql_field_count(connection);
        MYSQL_ROW row;

        Misc::Console(DEBUG_DB, "\n\nExecuteQuery(): test3 : count: %i\n", count);
        int i = 0;

        while (row = mysql_fetch_row(res))
        {
            AddRow(row, count);
            i++;
        }

        Misc::Console(DEBUG_DB, "\n\nExecuteQuery(): test4 : i: %i\n", i);

        mysql_free_result(res);
    }

    Misc::Console(DEBUG_DB, "\n\nExecuteQuery(): test5: rows.size(): %i\n", (int)rows.size());

    return rows.size();
}

int Database::ExecuteQuery(const std::string & query)
{
    actualQuery = query;

    return ExecuteQuery();
}

int Database::ExecutePQuery(const char * format, ...)
{
    va_list ap;
    char szQuery[MAX_QUERY_LEN];
    va_start(ap, format);
    int res = vsnprintf(szQuery, MAX_QUERY_LEN, format, ap);
    va_end(ap);

    if (res == DB_RESULT_ERROR)
        return DB_RESULT_ERROR;

    SetQuery(szQuery);

    return ExecuteQuery();
}

std::string Database::EscapeString(const char * str)
{
    size_t len = strlen(str);
    char * tmp;
    tmp = new char[len];

    mysql_real_escape_string(connection, tmp, str, len);

    std::string tmpString = tmp;

    delete [] tmp;

    return tmpString;
}

std::string Database::EscapeString(const std::string & str)
{
    return EscapeString(str.c_str());
}

std::string Database::EscapeString(const Wt::WString & str)
{
    return EscapeString(str.toUTF8().c_str());
}

const char * Database::GetError()
{
    return mysql_error(connection);
}

unsigned int Database::GetErrNo()
{
    return mysql_errno(connection);
}

void Database::Clear()
{
    for (std::list<DatabaseRow*>::iterator itr = rows.begin(); itr != rows.end(); ++itr)
        delete *itr;

    rows.clear();
}

void Database::AddRow(MYSQL_ROW row, int count)
{
    if (row && count > 0)
        rows.push_back(new DatabaseRow(row, count));
}

DatabaseRow * Database::GetRow(uint32 index)
{
    if (index >= rows.size())
        return NULL;

    std::list<DatabaseRow*>::iterator itr = rows.begin();
    std::advance(itr, index);

    return *itr;
}

DatabaseRow * Database::GetRow()
{
    if (rows.empty())
        return NULL;

    return rows.front();
}

std::list<DatabaseRow*> Database::GetRows()
{
    return rows;
}
