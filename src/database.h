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

#ifndef DATABASE_H_INCLUDED
#define DATABASE_H_INCLUDED

#ifdef WIN32
#include <winsock2.h>
//#include "mysql.h"
#endif

#include <list>
#include <mysql/mysql.h>

#include "defines.h"

#define MAX_QUERY_LEN 512

// connect to database, set query, escape query + execute query
struct DatabaseField
{
    DatabaseField(){}
    ~DatabaseField(){}

    Wt::WString value;

    Wt::WString GetWString();
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
    ~Database();

    void SetQuery(const std::string & query);           /// set query to execute
    bool SetPQuery(const char *format, ...);            /// set query to execute

    bool Connect(std::string host, std::string login, std::string pass, unsigned int port, std::string db); // connects to db
    void Disconnect();
    bool SelectDatabase(std::string db);

    std::string EscapeString(const char * str);         /// escape given string
    std::string EscapeString(const std::string & str);  /// escape given string
    std::string EscapeString(const Wt::WString & str);  /// escape given string

    int ExecuteQuery();                                 /// execute setted query and returns row count
    int ExecuteQuery(const std::string & query);        /// execute given query and return row count
    int ExecutePQuery(const char * format, ...);

    const char * GetError();                            /// get mysql error
    unsigned int GetErrNo();                            /// get mysql error number

    void AddRow(MYSQL_ROW row, int count);              /// add new row

    void Clear();                                       /// clear (+ delete from memory) result

    int GetRowsCount() { return rows.size(); }          /// return rows count
    DatabaseRow * GetRow(uint32 index);                 /// returns row from given index
    DatabaseRow * GetRow();                             /// returns first row
    std::list<DatabaseRow*> GetRows();                  /// returns all rows
    std::string GetQuery() { return actualQuery; }      /// returns actual query

    void SetLogging(bool enabled) { loggingEnabled = enabled; }

private:
    MYSQL * connection;                                 /// mysql connection
    std::string actualQuery;                            /// actual query
    std::list<DatabaseRow*> rows;                       /// query result

    bool loggingEnabled;                                /// queries should be logged ?
};

#endif // DATABASE_H_INCLUDED
