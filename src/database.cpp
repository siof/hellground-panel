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
            return LVL_PLAYER;
        case LVL_GM_TRIAL:
            return LVL_GM_TRIAL;
        case LVL_GM_HELPER:
            return LVL_GM_HELPER;
        case LVL_GM_HEAD:
            return LVL_GM_HEAD;
        case LVL_ADM:
            return LVL_ADM;
    }

    return LVL_PLAYER;
}

/// DatabaseRow
DatabaseRow::DatabaseRow(MYSQL_ROW row, int count)
{
    #ifdef DEBUG_DATABASE
    printf("Call DatabaseRow::DatabaseRow(MYSQL_ROW row, int count = %i)\n", count);
    #endif
    this->count = count;
    fields = new DatabaseField[count];
    for (int i = 0; i < count; ++i)
    {
        fields[i].value = WString::fromUTF8(row[i] ? row[i] : "");
        #ifdef DEBUG_DATABASE
        std::cout << i << " : " << (row[i] ? row[i] : "") << std::endl;
        #endif
    }
}

DatabaseRow::~DatabaseRow()
{
    delete [] fields;
}

/// Database

Database::Database()
{
    connection = mysql_init(NULL);
}

Database::Database(std::string host, std::string login, std::string pass, unsigned int port, std::string db)
{
    connection = mysql_init(NULL);
    Connect(host, login, pass, port, db);
}

Database::~Database()
{
    Clear();
    mysql_close(connection);
    connection = NULL;
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

    if (res == -1)
        return false;

    SetQuery(szQuery);
    return true;
}

bool Database::Connect(std::string host, std::string login, std::string pass, unsigned int port, std::string db)
{
    return mysql_real_connect(connection, host.c_str(), login.c_str(), pass.c_str(), db.c_str(), port, NULL, 0) != NULL;
}

bool Database::SelectDatabase(std::string db)
{
    return mysql_select_db(connection, db.c_str()) != NULL;
}

void Database::EscapeQuery()
{
    char * tmpTo = new char[actualQuery.size()];
    mysql_real_escape_string(connection, tmpTo, actualQuery.c_str(), actualQuery.size());
    actualQuery = tmpTo;
    delete [] tmpTo;
    tmpTo = NULL;
}

int Database::ExecuteQuery()
{
    #ifdef DEBUG_DATABASE
    printf("\nCall int Database::ExecuteQuery() : actualQuery: %s", actualQuery.c_str());
    #endif

    Clear();
    #ifdef DEBUG_DATABASE
    printf("\n\nExecuteQuery(): test1\n");
    #endif
    if (mysql_query(connection, actualQuery.c_str()))
        return -1;

    #ifdef DEBUG_DATABASE
    printf("\n\nExecuteQuery(): test2\n");
    #endif
    MYSQL_RES * res;

    if (res = mysql_store_result(connection))
    {
        unsigned int count = mysql_field_count(connection);
        MYSQL_ROW row;

        #ifdef DEBUG_DATABASE
        printf("\n\nExecuteQuery(): test4 : count: %i\n", count);
        int i = 0;
        #endif
        while (row = mysql_fetch_row(res))
        {
            AddRow(row, count);
            #ifdef DEBUG_DATABASE
            i++;
            #endif
        }

        #ifdef DEBUG_DATABASE
        printf("\n\nExecuteQuery(): test5 : i: %i\n", i);
        #endif

        mysql_free_result(res);
    }

    #ifdef DEBUG_DATABASE
    printf("\n\nExecuteQuery(): test6: rows.size(): %i\n", (int)rows.size());
    #endif

    return rows.size();
}

int Database::ExecuteQuery(std::string query, bool escape)
{
    actualQuery = query;
    if (escape)
        EscapeQuery();

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

DatabaseRow * Database::GetRow(int index)
{
    if (uint32(index) >= rows.size())
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
