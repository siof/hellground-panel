/*
*    HG Players Panel - web panel for HellGround server Players
*    Copyright (C) 2011 HellGround Team : Siof, lukaasm,
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU Affero General Public License as
*    published by the Free Software Foundation, either version 3 of the
*    License, or (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Affero General Public License for more details.
*
*    You should have received a copy of the GNU Affero General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DATABASE_H_INCLUDED
#define DATABASE_H_INCLUDED

#include "defines.h"
#ifdef WIN32
#include <winsock.h>
#include "mysql.h"
#else
#include <mysql/mysql.h>
#endif

// connect to database, set query, escape query + execute query
struct DatabaseField
{
    DatabaseField(){}
    ~DatabaseField(){}

    std::string value;

    WString GetWString();
    const char * GetCString();
    std::string GetString();
    uint64 GetUInt64();
    uint32 GetUInt32();
    int GetInt();
    bool GetBool();
    AccountLevel GetAccountLevel();
};

struct DatabaseRow
{
    DatabaseRow(MYSQL_ROW row, int count);
    ~DatabaseRow();

    DatabaseField * fields;
    int count;
};

class Database
{
public:
    Database();
    Database(std::string host, std::string login, std::string pass, std::string db, unsigned int port);
    ~Database();

    void SetQuery(std::string query);
    bool Connect(std::string host, std::string login, std::string pass, std::string db, unsigned int port);
    bool SelectDatabase(std::string db);
    void EscapeQuery();
    std::string EscapeString(std::string str);
    std::string EscapeString(WString str);
    int ExecuteQuery();
    int ExecuteQuery(std::string query, bool escape = true);
    const char * GetError();
    void AddRow(MYSQL_ROW row, int count);
    void Clear();
    int GetRowsCount() { return rows.size(); }
    DatabaseRow * GetRow(int index);
    std::list<DatabaseRow*> GetRows();
    std::string GetQuery() { return actualQuery; }
private:
    MYSQL * connection;
    std::string actualQuery;
    std::list<DatabaseRow*> rows;
};

#endif // DATABASE_H_INCLUDED
