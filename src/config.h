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

/********************************************//**
 * \addtogroup Configuration
 * \{
 *
 * \file config.h.dist
 *
 ***********************************************/


#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include <mutex>

#include <Wt/WString>

#include "defines.h"

enum StringConfig
{
    CONFIG_DEFAULT_TEMPLATE_NAME        = 0,
    CONFIG_DEFAULT_TEMPLATE_STYLE_PATH,
    CONFIG_DEFAULT_TEMPLATE_TMPLT_PATH,
    CONFIG_LANGUAGE_FILE_PATH,

    CONFIG_LOGIN_VALIDATOR,

    CONFIG_MAIL_FROM,
    CONFIG_MAIL_HOST,
    CONFIG_MAIL_USER,
    CONFIG_MAIL_PASSWORD,

    CONFIG_EMAIL_HIDE_CHARACTER,

    CONFIG_DB_PANEL_HOST,
    CONFIG_DB_PANEL_LOGIN,
    CONFIG_DB_PANEL_PASSWORD,
    CONFIG_DB_PANEL_NAME,

    CONFIG_DB_ACCOUNTS_HOST,
    CONFIG_DB_ACCOUNTS_LOGIN,
    CONFIG_DB_ACCOUNTS_PASSWORD,
    CONFIG_DB_ACCOUNTS_NAME,

    STRING_CONFIG_COUNT
};

enum BoolConfig
{
    CONFIG_DB_SHOW_ERRORS           = 0,

    CONFIG_REGISTRATION_ENABLED,

    CONFIG_EMAIL_HIDE_DOMAIN,

    CONFIG_ALLOW_TWO_SIDE_ACCOUNTS,

    BOOL_CONFIG_COUNT
};

enum IntConfig
{
    CONFIG_OPTIONS_DEBUG            = 0,
    CONFIG_OPTIONS_LOG,

    CONFIG_EMAIL_SHOW_CHAR_COUNT,
    CONFIG_EMAIL_HIDE_CHAR_COUNT,

    CONFIG_PASSWORD_LENGTH_MIN,
    CONFIG_PASSWORD_LENGTH_MAX,
    CONFIG_PASSWORD_GEN_ASCII_START,
    CONFIG_PASSWORD_GEN_ASCII_STOP,

    CONFIG_DB_PANEL_PORT,
    CONFIG_DB_ACCOUNTS_PORT,

    CONFIG_REALMS_COUNT,

    CONFIG_MAX_CHARACTERS_PER_REALM,
    CONFIG_MAX_CHARACTERS_PER_ACCOUNT,

    CONFIG_INTERVAL_UPDATE_CHARACTERS,

    CONFIG_INTERVAL_VOTE,

    CONFIG_ACTIVITY_LIMIT_PANEL,
    CONFIG_ACTIVITY_LIMIT_SERVER,

    CONFIG_STARTING_EXPANSION,

    INT_CONFIG_COUNT
};

class Config
{
public:
    static Config & Instance();

    void ReadConfig();

    const std::string & GetConfig(StringConfig option) const;
    int GetConfig(IntConfig option) const;
    bool GetConfig(BoolConfig option) const;

    void SetConfig(StringConfig option, const std::string & value);
    void SetConfig(IntConfig option, const int & value);
    void SetConfig(BoolConfig option, bool value);

    Location GetStartingLocation(CharacterRaces race) const;
    void SetStartingLocation(CharacterRaces race, Location & loc);

    const RealmInformations & GetRealmInformations(int entry) const;

private:
    Config() {}
    Config(const Config &) {}

    std::string configString[STRING_CONFIG_COUNT];
    int configInt[INT_CONFIG_COUNT];
    bool configBool[BOOL_CONFIG_COUNT];

    Location raceLocations[10];

    RealmInformations * realmInfos;

    static volatile Config * _config;
    static std::mutex _createMutex;
    std::mutex _configMutex;
};

#define sConfig Config::Instance()

#define DB_ACCOUNTS_DATA sConfig.GetConfig(CONFIG_DB_ACCOUNTS_HOST), sConfig.GetConfig(CONFIG_DB_ACCOUNTS_LOGIN), \
                         sConfig.GetConfig(CONFIG_DB_ACCOUNTS_PASSWORD),\
                         sConfig.GetConfig(CONFIG_DB_ACCOUNTS_PORT), sConfig.GetConfig(CONFIG_DB_ACCOUNTS_NAME)

#define DB_PANEL_DATA   sConfig.GetConfig(CONFIG_DB_PANEL_HOST), sConfig.GetConfig(CONFIG_DB_PANEL_LOGIN), \
                        sConfig.GetConfig(CONFIG_DB_PANEL_PASSWORD),\
                        sConfig.GetConfig(CONFIG_DB_PANEL_PORT), sConfig.GetConfig(CONFIG_DB_PANEL_NAME)

#define DB_REALM_DATA(a)    sConfig.GetRealmInformations(a).dbHost, sConfig.GetRealmInformations(a).dbLogin, \
                            sConfig.GetRealmInformations(a).dbPass, sConfig.GetRealmInformations(a).dbPort, \
                            sConfig.GetRealmInformations(a).dbName

#endif // CONFIG_H_INCLUDED

/**< \} */
