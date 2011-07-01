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
//#include "mysql.h"
#endif

#include <mysql/mysql.h>


#define MAX_QUERY_LEN 256

// connect to database, set query, escape query + execute query
struct DatabaseField
{
    DatabaseField(){}
    ~DatabaseField(){}

    WString value;

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
    Database(std::string host, std::string login, std::string pass, unsigned int port, std::string db);
    ~Database();

    void SetQuery(std::string query);   // set query to execute
    bool SetPQuery(const char *format, ...);
    bool Connect(std::string host, std::string login, std::string pass, unsigned int port, std::string db); // connects to db
    bool SelectDatabase(std::string db);
    void EscapeQuery(); // escape actual query
    std::string EscapeString(std::string str);  // escape given string
    std::string EscapeString(WString str);      // escape given string
    int ExecuteQuery(); // execute setted query and returns row count
    int ExecuteQuery(std::string query, bool escape = true);    // execute given query and return row count
    const char * GetError();    // get mysql error
    void AddRow(MYSQL_ROW row, int count);  // add new row
    void Clear();   // clear (+ delete from memory) result
    int GetRowsCount() { return rows.size(); }  // return rows count
    DatabaseRow * GetRow(int index);    // returns row from given index
    DatabaseRow * GetRow();             // returns first row
    std::list<DatabaseRow*> GetRows();  // returns all rows
    std::string GetQuery() { return actualQuery; }
private:
    MYSQL * connection;             // mysql connection
    std::string actualQuery;        // actual query
    std::list<DatabaseRow*> rows;   // query result
};

#endif // DATABASE_H_INCLUDED
