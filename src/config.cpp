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

#include <iostream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

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

    return * const_cast<Config*>(_config);
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

void Config::ReadConfig()
{
    if (!_configMutex.try_lock())
        return;

    std::cout << "Preparing config" << std::endl;

    // prepare config options
    boost::property_tree::ptree pt;
    boost::property_tree::xml_parser::read_xml("config.xml", pt);

    std::cout << "Setting config values" << std::endl;

    std::cout << "    template" << std::endl;
    SetConfig(CONFIG_DEFAULT_TEMPLATE_NAME, pt.get("template.name", "default"));
    SetConfig(CONFIG_DEFAULT_TEMPLATE_STYLE_PATH, pt.get("template.path.style", "res/templates/default"));
    SetConfig(CONFIG_DEFAULT_TEMPLATE_TMPLT_PATH, pt.get("template.path.file", "res/templates/default"));

    std::cout << "    language" << std::endl;
    SetConfig(CONFIG_LANGUAGE_FILE_PATH, pt.get("language.file.path", "langs/panel"));

    std::cout << "    validator" << std::endl;
    SetConfig(CONFIG_LOGIN_VALIDATOR, pt.get("validator.login", "[a-zA-Z0-9_-]{6,16}"));

    std::cout << "    mail" << std::endl;
    SetConfig(CONFIG_MAIL_FROM, pt.get("mail.from", "none@none.none"));
    SetConfig(CONFIG_MAIL_HOST, pt.get("mail.host", "localhost"));
    SetConfig(CONFIG_MAIL_USER, pt.get("mail.user", ""));
    SetConfig(CONFIG_MAIL_PASSWORD, pt.get("mail.password", ""));

    std::cout << "    database.panel" << std::endl;
    SetConfig(CONFIG_DB_PANEL_HOST, pt.get("database.panel.host", "localhost"));
    SetConfig(CONFIG_DB_PANEL_LOGIN, pt.get("database.panel.login", "panel"));
    SetConfig(CONFIG_DB_PANEL_PASSWORD, pt.get("database.panel.password", "panel"));
    SetConfig(CONFIG_DB_PANEL_PORT, pt.get("database.panel.port", 3306));
    SetConfig(CONFIG_DB_PANEL_NAME, pt.get("database.panel.name", "panel"));

    std::cout << "    database.accounts" << std::endl;
    SetConfig(CONFIG_DB_ACCOUNTS_HOST, pt.get("database.accounts.host", "localhost"));
    SetConfig(CONFIG_DB_ACCOUNTS_LOGIN, pt.get("database.accounts.login", "panel"));
    SetConfig(CONFIG_DB_ACCOUNTS_PASSWORD, pt.get("database.accounts.password", "panel"));
    SetConfig(CONFIG_DB_ACCOUNTS_PORT, pt.get("database.accounts.port", 3306));
    SetConfig(CONFIG_DB_ACCOUNTS_NAME, pt.get("database.accounts.name", "accounts"));

    std::cout << "    database" << std::endl;
    SetConfig(CONFIG_DB_SHOW_ERRORS, pt.get("database.show.errors", true));

    std::cout << "    email" << std::endl;
    SetConfig(CONFIG_EMAIL_SHOW_CHAR_COUNT, pt.get("email.show.count", 2));
    SetConfig(CONFIG_EMAIL_HIDE_CHAR_COUNT, pt.get("email.hide.count", 4));
    SetConfig(CONFIG_EMAIL_HIDE_CHARACTER, pt.get("email.hide.character", "*"));
    SetConfig(CONFIG_EMAIL_HIDE_DOMAIN, pt.get("email.hide.domain", true));

    std::cout << "    server" << std::endl;
    SetConfig(CONFIG_ALLOW_TWO_SIDE_ACCOUNTS, pt.get("server.allow.two-side-accounts", false));
    SetConfig(CONFIG_REGISTRATION_ENABLED, pt.get("server.registration.enabled", true));
    SetConfig(CONFIG_REALMS_COUNT, pt.get("server.realms.count", 1));
    SetConfig(CONFIG_MAX_CHARACTERS_PER_REALM, pt.get("server.max.characters.per.realm", 50));
    SetConfig(CONFIG_MAX_CHARACTERS_PER_ACCOUNT, pt.get("server.max.characters.per.account", 10));
    SetConfig(CONFIG_STARTING_EXPANSION, pt.get("server.starting.expansion", 1));

    std::cout << "    options" << std::endl;
    SetConfig(CONFIG_OPTIONS_DEBUG, pt.get("options.debug", int(DEBUG_NONE)));
    SetConfig(CONFIG_OPTIONS_LOG, pt.get("options.log", int(LOG_DB)));

    std::cout << "    password" << std::endl;
    SetConfig(CONFIG_PASSWORD_LENGTH_MIN, pt.get("password.length.min", 6));
    SetConfig(CONFIG_PASSWORD_LENGTH_MAX, pt.get("password.length.max", 16));
    SetConfig(CONFIG_PASSWORD_GEN_ASCII_START, pt.get("password.generation.ascii.start", 33));
    SetConfig(CONFIG_PASSWORD_GEN_ASCII_STOP, pt.get("password.generation.ascii.stop", 126));

    std::cout << "    interval" << std::endl;
    SetConfig(CONFIG_INTERVAL_UPDATE_CHARACTERS, pt.get("interval.update.characters", 5));
    SetConfig(CONFIG_INTERVAL_VOTE, pt.get("interval.vote", 12));

    std::cout << "    activity" << std::endl;
    SetConfig(CONFIG_ACTIVITY_LIMIT_PANEL, pt.get("activity.limit.panel", 100));
    SetConfig(CONFIG_ACTIVITY_LIMIT_SERVER, pt.get("activity.limit.server", 100));

    Location loc;

    loc.mapId = pt.get("race.location.Human.map",   0);
    loc.zone = pt.get("race.location.Human.zone",   12);
    loc.posX = pt.get("race.location.Human.x",      -8949.95);
    loc.posY = pt.get("race.location.Human.y",      -132.493);
    loc.posZ = pt.get("race.location.Human.z",      83.5312);
    SetStartingLocation(RACE_HUMAN, loc);

    loc.mapId = pt.get("race.location.Orc.map",     1);
    loc.zone = pt.get("race.location.Orc.zone",     14);
    loc.posX = pt.get("race.location.Orc.x",        -618.518);
    loc.posY = pt.get("race.location.Orc.y",        -4251.67);
    loc.posZ = pt.get("race.location.Orc.z",        38.718);
    SetStartingLocation(RACE_ORC, loc);

    loc.mapId = pt.get("race.location.Dwarf.map",   0);
    loc.zone = pt.get("race.location.Dwarf.zone",   1);
    loc.posX = pt.get("race.location.Dwarf.x",      -6240.32);
    loc.posY = pt.get("race.location.Dwarf.y",      331.033);
    loc.posZ = pt.get("race.location.Dwarf.z",      382.758);
    SetStartingLocation(RACE_DWARF, loc);

    loc.mapId = pt.get("race.location.NightElf.map",    1);
    loc.zone = pt.get("race.location.NightElf.zone",    141);
    loc.posX = pt.get("race.location.NightElf.x",       10311.3);
    loc.posY = pt.get("race.location.NightElf.y",       832.463);
    loc.posZ = pt.get("race.location.NightElf.z",       1326.41);
    SetStartingLocation(RACE_NIGHT_ELF, loc);

    loc.mapId = pt.get("race.location.Undead.map",  0);
    loc.zone = pt.get("race.location.Undead.zone",  85);
    loc.posX = pt.get("race.location.Undead.x",     1676.71);
    loc.posY = pt.get("race.location.Undead.y",     1678.31);
    loc.posZ = pt.get("race.location.Undead.z",     121.67);
    SetStartingLocation(RACE_UNDEAD, loc);

    loc.mapId = pt.get("race.location.Tauren.map",  1);
    loc.zone = pt.get("race.location.Tauren.zone",  215);
    loc.posX = pt.get("race.location.Tauren.x",     -2917.58);
    loc.posY = pt.get("race.location.Tauren.y",     -257.98);
    loc.posZ = pt.get("race.location.Tauren.z",     52.9968);
    SetStartingLocation(RACE_TAUREN, loc);

    loc.mapId = pt.get("race.location.Gnome.map",   0);
    loc.zone = pt.get("race.location.Gnome.zone",   1);
    loc.posX = pt.get("race.location.Gnome.x",      -6240.32);
    loc.posY = pt.get("race.location.Gnome.y",      331.033);
    loc.posZ = pt.get("race.location.Gnome.z",      382.758);
    SetStartingLocation(RACE_GNOME, loc);

    loc.mapId = pt.get("race.location.Troll.map",   1);
    loc.zone = pt.get("race.location.Troll.zone",   14);
    loc.posX = pt.get("race.location.Troll.x",      -618.518);
    loc.posY = pt.get("race.location.Troll.y",      -4251.67);
    loc.posZ = pt.get("race.location.Troll.z",      38.718);
    SetStartingLocation(RACE_TROLL, loc);

    loc.mapId = pt.get("race.location.BloodElf.map",    530);
    loc.zone = pt.get("race.location.BloodElf.zone",    3431);
    loc.posX = pt.get("race.location.BloodElf.x",       10349.6);
    loc.posY = pt.get("race.location.BloodElf.y",       -6357.29);
    loc.posZ = pt.get("race.location.BloodElf.z",       33.4026);
    SetStartingLocation(RACE_BLOOD_ELF, loc);

    loc.mapId = pt.get("race.location.Draenei.map", 530);
    loc.zone = pt.get("race.location.Draenei.zone", 3526);
    loc.posX = pt.get("race.location.Draenei.x",    -3961.64);
    loc.posY = pt.get("race.location.Draenei.y",    -13931.2);
    loc.posZ = pt.get("race.location.Draenei.z",    100.615);
    SetStartingLocation(RACE_DRAENEI, loc);

    std::cout << "Race locations setted" << std::endl;

    int realmCount = GetConfig(CONFIG_REALMS_COUNT);
    realmInfos = new RealmInformations[realmCount];

    std::cout << "Parsing realms options" << std::endl;

    for (int i = 0; i < realmCount; ++i)
    {
        std::string partialOption = Misc::GetFormattedString("server.realms.info.%i.", i);
        std::string fullOption = partialOption + "name";
        realmInfos[i].name = pt.get(fullOption, "None");
        realmInfos[i].statusUrl = pt.get(partialOption + "statusurl", "http://localhost/status.prsr");
        realmInfos[i].additionalInfo = pt.get(partialOption + "additional", "");
        realmInfos[i].realmId = pt.get(partialOption + "id", realmCount);
        realmInfos[i].dbHost = pt.get(partialOption + "dbhost", "localhost");
        realmInfos[i].dbLogin = pt.get(partialOption + "dblogin", "panel");
        realmInfos[i].dbPass = pt.get(partialOption + "dbpass", "panel");
        realmInfos[i].dbPort = pt.get(partialOption + "dbport", 3306);
        realmInfos[i].dbName = pt.get(partialOption + "dbname", "panel");

        std::cout << "    Parsed options for realm id " << i << " and partial option " << partialOption << std::endl;
    }

    std::cout << "Realms options parsed" << std::endl;

    _configMutex.unlock();
}

volatile Config * Config::_config = nullptr;
std::mutex Config::_createMutex;
