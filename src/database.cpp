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

#include "database.h"
#include <cstdarg>


/// DatabaseField

WString DatabaseField::GetWString()
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
    SSCANF(value.toUTF8().c_str(), "%u", &tmp);
    return tmp;
}

uint32 DatabaseField::GetUInt32()
{
    uint32 tmp = 0;
    SSCANF(value.toUTF8().c_str(), "%u", &tmp);
    return tmp;
}

int DatabaseField::GetInt()
{
    int tmp = 0;
    SSCANF(value.toUTF8().c_str(), "%i", &tmp);
    return tmp;
}

bool DatabaseField::GetBool()
{
    int tmp = 0;
    SSCANF(value.toUTF8().c_str(), "%i", &tmp);

    if (tmp)
        return true;

    return false;
}

AccountLevel DatabaseField::GetAccountLevel()
{
    int tmp = 0;
    SSCANF(value.toUTF8().c_str(), "%i", &tmp);

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
    console(DEBUG_DB, "Call DatabaseRow::DatabaseRow(MYSQL_ROW row, int count = %i)\n", count);

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
}

Database::~Database()
{
    Disconnect();
    Clear();
}

void Database::SetQuery(std::string query)
{
    actualQuery = query;
}

bool Database::SetPQuery(const char *format, ...)
{
    va_list ap;
    char szQuery[MAX_QUERY_LEN];
    va_start(ap, format);
    int res = VSNPRINTF(szQuery, MAX_QUERY_LEN, format, ap);
    va_end(ap);

    if (res == RETURN_ERROR)
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
    console(DEBUG_DB, "\nCall int Database::ExecuteQuery() : actualQuery: %s", actualQuery.c_str());

    Clear();

    console(DEBUG_DB, "\n\nExecuteQuery(): test1\n");

    Log(LOG_DB_QUERY, "DB Query execute: %s", actualQuery.c_str());

    if (mysql_query(connection, actualQuery.c_str()))
    {
        Log(LOG_DB_ERRORS, "DB Query error ! Error [%i]: %s", GetErrNo(), GetError());
        return RETURN_ERROR;
    }

    console(DEBUG_DB, "\n\nExecuteQuery(): test2\n");

    MYSQL_RES * res;

    if (res = mysql_store_result(connection))
    {
        unsigned int count = mysql_field_count(connection);
        MYSQL_ROW row;

        console(DEBUG_DB, "\n\nExecuteQuery(): test3 : count: %i\n", count);
        int i = 0;

        while (row = mysql_fetch_row(res))
        {
            AddRow(row, count);
            i++;
        }

        console(DEBUG_DB, "\n\nExecuteQuery(): test4 : i: %i\n", i);

        mysql_free_result(res);
    }

    console(DEBUG_DB, "\n\nExecuteQuery(): test5: rows.size(): %i\n", (int)rows.size());

    return rows.size();
}

int Database::ExecuteQuery(std::string query)
{
    actualQuery = query;

    return ExecuteQuery();
}

int Database::ExecutePQuery(const char * format, ...)
{
    va_list ap;
    char szQuery[MAX_QUERY_LEN];
    va_start(ap, format);
    int res = VSNPRINTF(szQuery, MAX_QUERY_LEN, format, ap);
    va_end(ap);

    if (res == RETURN_ERROR)
        return RETURN_ERROR;

    SetQuery(szQuery);

    return ExecuteQuery();
}

std::string Database::EscapeString(std::string str)
{
    char * tmp;
    tmp = new char[str.size()];

    mysql_real_escape_string(connection, tmp, str.c_str(), str.size());

    std::string tmpString = tmp;

    delete [] tmp;

    return tmpString;
}

std::string Database::EscapeString(WString str)
{
    return EscapeString(str.toUTF8());
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
    advance(itr, index);

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
