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

#include "config.h"

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include "misc.h"
#include "miscCharacter.h"

Config & Config::Instance()
{
    // if config wasn't initialized yet
    if (_config == nullptr)
    {
        // lock mutex to prevent creating config twice
        _createMutex.lock();

        // check for null once more - we don't want to create multiple configs
        if (_config == nullptr)
            // create config
            _config = new Config();

        // and finally unlock mutex
        _createMutex.unlock();
    }

    return * _config;
}

const std::string & Config::GetConfig(StringConfig option) const
{
    return configString[option];
}

int Config::GetConfig(IntConfig option) const
{
    return configInt[option];
}

bool Config::GetConfig(BoolConfig option) const
{
    return configBool[option];
}

void Config::SetConfig(StringConfig option, const std::string & value)
{
    configString[option] = value;
}

void Config::SetConfig(IntConfig option, const int & value)
{
    configInt[option] = value;
}

void Config::SetConfig(BoolConfig option, bool value)
{
    configBool[option] = value;
}

Location Config::GetStartingLocation(CharacterRaces race) const
{
    // array is from 0 but races from 1
    int raceIndex = race - 1;

    // 9 is empty
    if (raceIndex > 7)
        --raceIndex;

    return raceLocations[raceIndex];
}

void Config::SetStartingLocation(CharacterRaces race, Location & loc)
{
    // array is from 0 but races from 1
    int raceIndex = race - 1;

    // 9 is empty
    if (raceIndex > 7)
        --raceIndex;

    raceLocations[raceIndex] = loc;
}

const RealmInformations & Config::GetRealmInformations(int entry) const
{
    if (entry < 0 || entry >= GetConfig(CONFIG_REALMS_COUNT))
        return realmInfos[0];

    return realmInfos[entry];
}

namespace bpo = boost::program_options;

#include <iostream>

void Config::ReadConfig()
{
    if (!_configMutex.try_lock())
        return;

    std::cout << "Preparing config" << std::endl;

    // prepare config options
    boost::program_options::options_description desc("");
    desc.add_options()
        ("template.name",       bpo::value<std::string>()->default_value("default"), "Default template name")
        ("template.style.path", bpo::value<std::string>()->default_value("res/templates/default"), "Path to default template style")
        ("template.file.path",  bpo::value<std::string>()->default_value("res/templates/default"), "Path to default template file")

        ("language.file.path",  bpo::value<std::string>()->default_value("langs/panel"), "Path to language file")

        ("validator.login",     bpo::value<std::string>()->default_value("[a-zA-Z0-9_-]{6,16}"), "Regex for login validation")

        ("mail.from",       bpo::value<std::string>()->default_value("none@none.none"), "Email address for From clause")
        ("mail.host",       bpo::value<std::string>()->default_value("localhost"), "Host for sending mails")
        ("mail.user",       bpo::value<std::string>()->default_value(""), "User for sending mails")
        ("mail.password",   bpo::value<std::string>()->default_value(""), "Password for sending mails")

        ("database.panel.host",     bpo::value<std::string>()->default_value("localhost"), "Host to panel database")
        ("database.panel.login",    bpo::value<std::string>()->default_value("panel"), "Login to panel database")
        ("database.panel.password", bpo::value<std::string>()->default_value("panel"), "Password to panel database")
        ("database.panel.port",     bpo::value<int>()->default_value(3306), "port to connect to DB")
        ("database.panel.name",     bpo::value<std::string>()->default_value("panel"), "Panel database name")

        ("database.accounts.host",  bpo::value<std::string>()->default_value("localhost"), "Host to Account database")
        ("database.accounts.login", bpo::value<std::string>()->default_value("panel"), "Login to accounts database")
        ("database.accounts.password", bpo::value<std::string>()->default_value("panel"), "Password to accounts database")
        ("database.accounts.port",  bpo::value<int>()->default_value(3306), "port to connect to DB")
        ("database.account.name",   bpo::value<std::string>()->default_value("accounts"), "Accounts database name")

        ("database.show.errors", bpo::value<bool>()->default_value(true), "Database errors should be displayed ?")

        ("email.show.char.count",   bpo::value<int>()->default_value(2), "How much email characters should be shown")
        ("email.hide.char.count",   bpo::value<int>()->default_value(4), "How much false characters should be shown")
        ("email.hide.character",    bpo::value<std::string>()->default_value("*"), "False email character to show")
        ("email.hide.domain",       bpo::value<bool>()->default_value(true), "Should we hide also email domain?")

        ("server.allow.two.side.accounts",  bpo::value<bool>()->default_value(false), "Server allows two side accounts ?")
        ("server.registration.enabled",     bpo::value<bool>()->default_value(true), "Enables account registration")
        ("server.realms.count",             bpo::value<int>()->default_value(1), "Realms count for this server")
        ("server.max.characters.per.realm", bpo::value<int>()->default_value(50), "Maximum characters count per realm")
        ("server.max.characters.per.account", bpo::value<int>()->default_value(10), "Maximum characters count per account")
        ("server.starting.expansion",       bpo::value<int>()->default_value(1), "Starting expansion for new accounts")

        ("options.debug",   bpo::value<int>()->default_value(DEBUG_NONE), "Enabled debug options (mask)")
        ("options.log",     bpo::value<int>()->default_value(LOG_DB), "Enabled log options (mask)")

        ("password.length.min", bpo::value<int>()->default_value(6), "Minimum length of password")
        ("password.length.max", bpo::value<int>()->default_value(16), "Maximum length of password")
        ("password.generation.ascii.start", bpo::value<int>()->default_value(33), "")
        ("password.generation.ascii.stop",  bpo::value<int>()->default_value(126), "")

        ("interval.update.characters",  bpo::value<int>()->default_value(5), "Minimum interval between charactes updates")
        ("interval.vote",               bpo::value<int>()->default_value(12), "Minimum interval between voting on same vote list")

        ("activity.limit.panel",    bpo::value<int>()->default_value(100), "")
        ("activity.limit.server",   bpo::value<int>()->default_value(100), "")

        ("race.location.Human.map", bpo::value<int>()->default_value(0), "Starting map for Human race")
        ("race.location.Human.zone", bpo::value<int>()->default_value(12), "Starting zone for Human race")
        ("race.location.Human.x",   bpo::value<float>()->default_value(-8949.95), "Starting x position for Human race")
        ("race.location.Human.y",   bpo::value<float>()->default_value(-132.493), "Starting y position for Human race")
        ("race.location.Human.z",   bpo::value<float>()->default_value(83.5312), "Startint z position for Human race")

        ("race.location.Orc.map",   bpo::value<int>()->default_value(1), "Starting map for Orc race")
        ("race.location.Orc.zone",  bpo::value<int>()->default_value(14), "Starting zone for Orc race")
        ("race.location.Orc.x",     bpo::value<float>()->default_value(-618.518), "Starting x position for Orc race")
        ("race.location.Orc.y",     bpo::value<float>()->default_value(-4251.67), "Starting y position for Orc race")
        ("race.location.Orc.z",     bpo::value<float>()->default_value(38.718), "Startint z position for Orc race")

        ("race.location.Dwarf.map",     bpo::value<int>()->default_value(0), "Starting map for Dwarf race")
        ("race.location.Dwarf.zone",    bpo::value<int>()->default_value(1), "Starting zone for Dwarf race")
        ("race.location.Dwarf.x",       bpo::value<float>()->default_value(-6240.32), "Starting x position for Dwarf race")
        ("race.location.Dwarf.y",       bpo::value<float>()->default_value(331.033), "Starting y position for Dwarf race")
        ("race.location.Dwarf.z",       bpo::value<float>()->default_value(382.758), "Startint z position for Dwarf race")

        ("race.location.NightElf.map",  bpo::value<int>()->default_value(1), "Starting map for Night Elf race")
        ("race.location.NightElf.zone", bpo::value<int>()->default_value(141), "Starting zone for Night Elf race")
        ("race.location.NightElf.x",    bpo::value<float>()->default_value(10311.3), "Starting x position for Night Elf race")
        ("race.location.NightElf.y",    bpo::value<float>()->default_value(832.463), "Starting y position for Night Elf race")
        ("race.location.NightElf.z",    bpo::value<float>()->default_value(1326.41), "Startint z position for Night Elf race")

        ("race.location.Undead.map",    bpo::value<int>()->default_value(0), "Starting map for Undead race")
        ("race.location.Undead.zone",   bpo::value<int>()->default_value(85), "Starting zone for Undead race")
        ("race.location.Undead.x",      bpo::value<float>()->default_value(1676.71), "Starting x position for Undead race")
        ("race.location.Undead.y",      bpo::value<float>()->default_value(1678.31), "Starting y position for Undead race")
        ("race.location.Undead.z",      bpo::value<float>()->default_value(121.67), "Startint z position for Undead race")

        ("race.location.Tauren.map",    bpo::value<int>()->default_value(1), "Starting map for Tauren race")
        ("race.location.Tauren.zone",   bpo::value<int>()->default_value(215), "Starting zone for Tauren race")
        ("race.location.Tauren.x",      bpo::value<float>()->default_value(-2917.58), "Starting x position for Tauren race")
        ("race.location.Tauren.y",      bpo::value<float>()->default_value(-257.98), "Starting y position for Tauren race")
        ("race.location.Tauren.z",      bpo::value<float>()->default_value(52.9968), "Startint z position for Tauren race")

        ("race.location.Gnome.map",     bpo::value<int>()->default_value(0), "Starting map for Gnome race")
        ("race.location.Gnome.zone",    bpo::value<int>()->default_value(1), "Starting zone for Gnome race")
        ("race.location.Gnome.x",       bpo::value<float>()->default_value(-6240.32), "Starting x position for Gnome race")
        ("race.location.Gnome.y",       bpo::value<float>()->default_value(331.033), "Starting y position for Gnome race")
        ("race.location.Gnome.z",       bpo::value<float>()->default_value(382.758), "Startint z position for Gnome race")

        ("race.location.Troll.map",     bpo::value<int>()->default_value(1), "Starting map for Troll race")
        ("race.location.Troll.zone",    bpo::value<int>()->default_value(14), "Starting zone for Troll race")
        ("race.location.Troll.x",       bpo::value<float>()->default_value(-618.518), "Starting x position for Troll race")
        ("race.location.Troll.y",       bpo::value<float>()->default_value(-4251.67), "Starting y position for Troll race")
        ("race.location.Troll.z",       bpo::value<float>()->default_value(38.718), "Startint z position for Troll race")

        ("race.location.BloodElf.map",  bpo::value<int>()->default_value(530), "Starting map for Blood Elf race")
        ("race.location.BloodElf.zone", bpo::value<int>()->default_value(3431), "Starting zone for Blood Elf race")
        ("race.location.BloodElf.x",    bpo::value<float>()->default_value(10349.6), "Starting x position for Blood Elf race")
        ("race.location.BloodElf.y",    bpo::value<float>()->default_value(-6357.29), "Starting y position for Blood Elf race")
        ("race.location.BloodElf.z",    bpo::value<float>()->default_value(33.4026), "Startint z position for Blood Elf race")

        ("race.location.Draenei.map",   bpo::value<int>()->default_value(530), "Starting map for Draenei race")
        ("race.location.Draenei.zone",  bpo::value<int>()->default_value(3526), "Starting zone for Draenei race")
        ("race.location.Draenei.x",     bpo::value<float>()->default_value(-3961.64), "Starting x position for Draenei race")
        ("race.location.Draenei.y",     bpo::value<float>()->default_value(-13931.2), "Starting y position for Draenei race")
        ("race.location.Draenei.z",     bpo::value<float>()->default_value(100.615), "Startint z position for Draenei race")
        ;

    std::cout << "Config prepared, preparing variables" << std::endl;

    boost::program_options::basic_parsed_options<char> parsed = boost::program_options::parse_config_file<char>("config.cnf", desc, true);
    boost::program_options::variables_map vm;
    boost::program_options::store(parsed, vm);
    boost::program_options::notify(vm);

    std::cout << "Setting config values" << std::endl;

    std::cout << "    template" << std::endl;
    // store parsed (and registered) config options
    SetConfig(CONFIG_DEFAULT_TEMPLATE_NAME, vm["template.name"].as<std::string>());
    SetConfig(CONFIG_DEFAULT_TEMPLATE_STYLE_PATH, vm["template.style.path"].as<std::string>());
    SetConfig(CONFIG_DEFAULT_TEMPLATE_TMPLT_PATH, vm["template.file.path"].as<std::string>());

    std::cout << "    language" << std::endl;
    SetConfig(CONFIG_LANGUAGE_FILE_PATH, vm["language.file.path"].as<std::string>());

    std::cout << "    validator" << std::endl;
    SetConfig(CONFIG_LOGIN_VALIDATOR, vm["validator.login"].as<std::string>());

    std::cout << "    mail" << std::endl;
    SetConfig(CONFIG_MAIL_FROM, vm["mail.from"].as<std::string>());
    SetConfig(CONFIG_MAIL_HOST, vm["mail.host"].as<std::string>());
    SetConfig(CONFIG_MAIL_USER, vm["mail.user"].as<std::string>());
    SetConfig(CONFIG_MAIL_PASSWORD, vm["mail.password"].as<std::string>());

    std::cout << "    database.panel" << std::endl;
    SetConfig(CONFIG_DB_PANEL_HOST, vm["database.panel.host"].as<std::string>());
    SetConfig(CONFIG_DB_PANEL_LOGIN, vm["database.panel.login"].as<std::string>());
    SetConfig(CONFIG_DB_PANEL_PASSWORD, vm["database.panel.password"].as<std::string>());
    SetConfig(CONFIG_DB_PANEL_PORT, vm["database.panel.port"].as<int>());
    SetConfig(CONFIG_DB_PANEL_NAME, vm["database.panel.name"].as<std::string>());

    std::cout << "    database.accounts" << std::endl;
    SetConfig(CONFIG_DB_ACCOUNTS_HOST, vm["database.accounts.host"].as<std::string>());
    SetConfig(CONFIG_DB_ACCOUNTS_LOGIN, vm["database.accounts.login"].as<std::string>());
    SetConfig(CONFIG_DB_ACCOUNTS_PASSWORD, vm["database.accounts.password"].as<std::string>());
    SetConfig(CONFIG_DB_ACCOUNTS_PORT, vm["database.accounts.port"].as<int>());
    SetConfig(CONFIG_DB_ACCOUNTS_NAME, vm["database.account.name"].as<std::string>());

    std::cout << "    database" << std::endl;
    SetConfig(CONFIG_DB_SHOW_ERRORS, vm["database.show.errors"].as<bool>());

    std::cout << "    email" << std::endl;
    SetConfig(CONFIG_EMAIL_SHOW_CHAR_COUNT, vm["email.show.char.count"].as<int>());
    SetConfig(CONFIG_EMAIL_HIDE_CHAR_COUNT, vm["email.hide.char.count"].as<int>());
    SetConfig(CONFIG_EMAIL_HIDE_CHARACTER, vm["email.hide.character"].as<std::string>());
    SetConfig(CONFIG_EMAIL_HIDE_DOMAIN, vm["email.hide.domain"].as<bool>());

    std::cout << "    server" << std::endl;
    SetConfig(CONFIG_ALLOW_TWO_SIDE_ACCOUNTS, vm["server.allow.two.side.accounts"].as<bool>());
    SetConfig(CONFIG_REGISTRATION_ENABLED, vm["server.registration.enabled"].as<bool>());
    SetConfig(CONFIG_REALMS_COUNT, vm["server.realms.count"].as<int>());
    SetConfig(CONFIG_MAX_CHARACTERS_PER_REALM, vm["server.max.characters.per.realm"].as<int>());
    SetConfig(CONFIG_MAX_CHARACTERS_PER_ACCOUNT, vm["server.max.characters.per.account"].as<int>());
    SetConfig(CONFIG_STARTING_EXPANSION, vm["server.starting.expansion"].as<int>());

    std::cout << "    options" << std::endl;
    SetConfig(CONFIG_OPTIONS_DEBUG, vm["options.debug"].as<int>());
    SetConfig(CONFIG_OPTIONS_LOG, vm["options.log"].as<int>());

    std::cout << "    password" << std::endl;
    SetConfig(CONFIG_PASSWORD_LENGTH_MIN, vm["password.length.min"].as<int>());
    SetConfig(CONFIG_PASSWORD_LENGTH_MAX, vm["password.length.max"].as<int>());
    SetConfig(CONFIG_PASSWORD_GEN_ASCII_START, vm["password.generation.ascii.start"].as<int>());
    SetConfig(CONFIG_PASSWORD_GEN_ASCII_STOP, vm["password.generation.ascii.stop"].as<int>());

    std::cout << "    interval" << std::endl;
    SetConfig(CONFIG_INTERVAL_UPDATE_CHARACTERS, vm["interval.update.characters"].as<int>());
    SetConfig(CONFIG_INTERVAL_VOTE, vm["interval.vote"].as<int>());

    std::cout << "    activity" << std::endl;
    SetConfig(CONFIG_ACTIVITY_LIMIT_PANEL, vm["activity.limit.panel"].as<int>());
    SetConfig(CONFIG_ACTIVITY_LIMIT_SERVER, vm["activity.limit.server"].as<int>());

    std::cout << "Registered values setted" << std::endl;

    // set starting locations
    {
        for (uint8 i = RACE_HUMAN; i <= RACE_DRAENEI; ++i)
        {
            if (i == 9)
                continue;

            Location loc;
            std::string baseOption = "race.location.";
            std::string race = Misc::Character::GetGlobalRaceName(i);

            std::cout << "Setting race locations for race " << race << std::endl;

            baseOption.append(race);

            std::string fullOption = baseOption + ".map";
            loc.mapId = vm[fullOption.c_str()].as<int>();

            fullOption = baseOption + ".zone";
            loc.zone = vm[fullOption.c_str()].as<int>();

            fullOption = baseOption + ".x";
            loc.posX = vm[fullOption.c_str()].as<float>();

            fullOption = baseOption + ".y";
            loc.posY= vm[fullOption.c_str()].as<float>();

            fullOption = baseOption + ".z";
            loc.posZ= vm[fullOption.c_str()].as<float>();

            SetStartingLocation(CharacterRaces(i), loc);
        }

        std::cout << "Race locations setted" << std::endl;
    }

    // prepare some holders for unregistered options
    realmInfos = new RealmInformations[GetConfig(CONFIG_REALMS_COUNT)];

    std::cout << "Parsing unregistered options" << std::endl;

    // parse and set rest (unregistered) options
    for (std::vector<boost::program_options::basic_option<char> >::iterator itr = parsed.options.begin(); itr != parsed.options.end(); ++itr)
    {
        const boost::program_options::basic_option<char> & option = *itr;

        if (option.unregistered)
        {
            std::vector<std::string> splited;
            boost::algorithm::split(splited, option.string_key, boost::algorithm::is_any_of("."), boost::algorithm::token_compress_on);

            // check for options: server.realm.info.X.option
            // where X is realm entry for array position
            if (splited.size() == 5 && splited[0] == "server" && splited[1] == "realm" && splited[2] == "info")
            {
                int realmEntry = 0;
                try
                {
                    realmEntry = boost::lexical_cast<int>(splited[3]);
                }
                catch (boost::bad_lexical_cast & e)
                {
                    Misc::Log(LOG_ALL, "Can't cast config option to realmEntry ! value: %s source type: %s target type: %s", splited[3].c_str(), e.source_type().name(), e.target_type().name());
                    continue;
                }

                std::string lowerOption = splited[4];
                boost::algorithm::to_lower(lowerOption);

                if (lowerOption == "name")
                    realmInfos[realmEntry].name = option.value.front();
                else if (lowerOption == "statusurl")
                    realmInfos[realmEntry].statusUrl = option.value.front();
                else if (lowerOption == "additional")
                {
                    realmInfos[realmEntry].additionalInfo = "";
                    for (std::vector<std::string>::const_iterator itr = option.value.begin(); itr != option.value.end(); ++itr)
                        realmInfos[realmEntry].additionalInfo.append(*itr).append(" ");
                }
                else if (lowerOption == "id")
                {
                    int realmId = 0;
                    try
                    {
                        realmId = boost::lexical_cast<int>(option.value.front());
                    }
                    catch (boost::bad_lexical_cast & e)
                    {
                        Misc::Log(LOG_ALL, "Can't cast config value to realmID ! value: %s source type: %s target type: %s", option.value.front().c_str(), e.source_type().name(), e.target_type().name());
                    }

                    realmInfos[realmEntry].id = realmId;
                }
                else if (lowerOption == "dbhost")
                    realmInfos[realmEntry].dbHost = option.value.front();
                else if (lowerOption == "dblogin")
                    realmInfos[realmEntry].dbLogin = option.value.front();
                else if (lowerOption == "dbpass")
                    realmInfos[realmEntry].dbPass = option.value.front();
                else if (lowerOption == "dbport")
                {
                    int dbPort = 3306;

                    try
                    {
                        dbPort = boost::lexical_cast<int>(option.value.front());
                    }
                    catch (boost::bad_lexical_cast & e)
                    {
                        Misc::Log(LOG_ALL, "Can't cast config value to dbPort ! value: %s source type: %s target type: %s", option.value.front().c_str(), e.source_type().name(), e.target_type().name());
                    }

                    realmInfos[realmEntry].dbPort = dbPort;
                }
                else if (lowerOption == "dbname")
                    realmInfos[realmEntry].dbName = option.value.front();
            }
        }
    }

    std::cout << "Unregistered options parsed" << std::endl;

    _configMutex.unlock();
}

Config * Config::_config = nullptr;
std::mutex Config::_createMutex;
