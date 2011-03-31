#include "database.h"


/// DatabaseField

WString DatabaseField::GetWString()
{
    return WString(value);
}

const char * DatabaseField::GetCString()
{
    return value.c_str();
}

std::string DatabaseField::GetString()
{
    return value;
}

uint64 DatabaseField::GetUInt64()
{
    uint64 tmp = 0;
    sscanf(value.c_str(), "%u", &tmp);
    return tmp;
}

int DatabaseField::GetInt()
{
    int tmp = 0;
    sscanf(value.c_str(), "%i", &tmp);
    return tmp;
}

bool DatabaseField::GetBool()
{
    int tmp = 0;
    sscanf(value.c_str(), "%i", &tmp);

    if (tmp)
        return true;

    return false;
}

AccountLevel DatabaseField::GetAccountLevel()
{
    int tmp = 0;
    sscanf(value.c_str(), "%i", &tmp);

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
    #ifdef DEBUG
    printf("Call DatabaseRow::DatabaseRow(MYSQL_ROW row, int count = %i)", count);
    #endif
    this->count = count;
    fields = new DatabaseField[count];
    for (int i = 0; i < count; ++i)
    {
        fields[i].value = (row[i] ? row[i] : "");
        #ifdef DEBUG
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

Database::Database(std::string host, std::string login, std::string pass, std::string db, unsigned int port)
{
    connection = mysql_init(NULL);
    Connect(host, login, pass, db, port);
}

Database::~Database()
{
    mysql_close(connection);
    connection = NULL;
}

void Database::SetQuery(std::string query)
{
    actualQuery = query;
}

bool Database::Connect(std::string host, std::string login, std::string pass, std::string db, unsigned int port)
{
    return mysql_real_connect(connection, host.c_str(), login.c_str(), pass.c_str(), db.c_str(), port, NULL, 0);
}

bool Database::SelectDatabase(std::string db)
{
    return mysql_select_db(connection, db.c_str());
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
    #ifdef DEBUG
    printf("Call int Database::ExecuteQuery()");
    #endif

    Clear();
    #ifdef DEBUG
    printf("\n\nExecuteQuery(): test1\n");
    #endif
    if (mysql_query(connection, actualQuery.c_str()))
        return -1;

    #ifdef DEBUG
    printf("\n\nExecuteQuery(): test2\n");
    #endif
    MYSQL_RES * res;

    if (res = mysql_store_result(connection))
    {
        unsigned int count = mysql_field_count(connection);
        MYSQL_ROW row;

        #ifdef DEBUG
        printf("\n\nExecuteQuery(): test4 : count: %i\n", count);
        int i = 0;
        #endif
        while (row = mysql_fetch_row(res))
        {
            AddRow(row, count);
            #ifdef DEBUG
            i++;
            #endif
        }

        #ifdef DEBUG
        printf("\n\nExecuteQuery(): test5 : i: %i\n", i);
        #endif

        mysql_free_result(res);
    }

    #ifdef DEBUG
    printf("\n\nExecuteQuery(): test6: rows.size(): %i\n", rows.size());
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
    char * tmpString;
    tmpString = new char[str.size()];

    mysql_real_escape_string(connection, tmpString, str.c_str(), str.size());

    return std::string(tmpString);
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
    if (index >= rows.size())
        return NULL;

    std::list<DatabaseRow*>::iterator itr = rows.begin();
    advance(itr, index);

    return *itr;
}

std::list<DatabaseRow*> Database::GetRows()
{
    return rows;
}