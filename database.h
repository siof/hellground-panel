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
    DatabaseRow * GetRow(int index);
    std::list<DatabaseRow*> GetRows();
    std::string GetQuery() { return actualQuery; }
private:
    MYSQL * connection;
    std::string actualQuery;
    std::list<DatabaseRow*> rows;
};

#endif // DATABASE_H_INCLUDED
