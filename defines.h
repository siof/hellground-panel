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

#ifndef DEFINES_H_INCLUDED
#define DEFINES_H_INCLUDED

#include "config.h"

// standard includes for most files
#include <WString>
#include <WBreak>
#include <WContainerWidget>
#include <WLineEdit>
#include <WPushButton>
#include <WText>
#include <WImage>
#include <WTable>

#include <cstdio>
#include <map>

using namespace Wt;

typedef uint64_t uint64;
typedef uint32_t uint32;
typedef uint16_t uint16;
typedef uint8_t  uint8;

#ifdef DEBUG
#include <iostream>
#endif

#ifdef WIN32
#define SSCANF      sscanf_s
#define VSNPRINTF   vsnprintf_s
#define OPENFILE    fopen_s
#define CLOSEFILE   fclose
#else
#define SSCANF      sscanf
#define VSNPRINTF   vsnprintf
#define OPENFILE    popen
#define CLOSEFILE   pclose
#endif

enum Lang
{
    LANG_PL = 0,    /**< Polish */
    LANG_EN,        /**< English */

    LANG_COUNT
};

enum AccountLevel
{
    LVL_NOT_LOGGED  = -1,   /**< not logged yet or new player */
    LVL_PLAYER      = 0,    /**< logged normal player with no special rights */
    LVL_GM_TRIAL    = 1,    /**< Game Master level 1 (Trial/Moderator) */
    LVL_GM_HELPER   = 2,    /**< Game Master level 2 (Helper) */
    LVL_GM_HEAD     = 3,    /**< Game Master level 3 (Head/Root Game Master) */
    LVL_ADM         = 4     /**< Server Administrator */
};

#define ACCOUNT_LEVEL_COUNT 6

/********************************************//**
 * \brief Represents single text loaded from DB
 *
 * \param textId - this text id (unique for each text)
 * \param texts[LANG_COUNT] - text for all supported languages
 *
 * Contains single text with unique id in all supported languages.
 *
 ***********************************************/

struct Text
{
    Text() : textId(0) {}
    Text(uint32 id) : textId(id) {}
    ~Text() {}

    WString& GetText(Lang lang)
    {
        return texts[lang];
    }

    uint32 textId;
    WString texts[LANG_COUNT];
};

/********************************************//**
 * \brief Contains session informations
 *
 * \param account login
 * \param account id
 * \param account password
 * \param account email
 * \param account create date
 * \param last logged in ip
 * \param current panel session ip
 * \param actual panel language
 * \param account level
 * \param language texts
 * \param ip lock
 * \param account expansion
 *
 * Contains players panel session informations.
 * Each player (logged or not) has his own session info.
 ***********************************************/

struct SessionInfo
{
    SessionInfo() : login(""), accid(0), pass(""), email(""), language(LANG_PL), accLvl(LVL_NOT_LOGGED), textMissing("Error ! Text missing !") {}
    ~SessionInfo() {}

    WString login;          /**< account login */
    uint64 accid;           /**< account id */
    WString pass;           /**< account password */
    WString email;          /**< account email */
    WString joinDate;       /**< account create time */
    WString lastIp;         /**< server last ip */
    WString sessionIp;      /**< panel session ip */
    Lang language;          /**< actual language */
    AccountLevel accLvl;    /**< account level (player, gm etc) */
    std::map<uint32 ,Text> langTexts;   /**< text depend on language loaded from database */
    bool locked;            /**< IP lock */
    int expansion;          /**< expansion */

    WString textMissing;

    WString& GetText(uint32 id)
    {
        std::map<uint32, Text>::iterator itr = langTexts.find(id);

        if (itr == langTexts.end())
            return textMissing;

        return itr->second.GetText(language);
    }
};

enum ErrorSlots
{
    ERROR_SLOT_BASE     = 0,
    ERROR_SLOT_ADDITIONAL,
    ERROR_SLOT_DB,
    ERROR_SLOT_INFO,
    ERROR_SLOT_COUNT
};

enum Expansion
{
    EXPANSION_PRETBC    = 0,    /**< Vanilla WoW (pre TBC, without expansion) */
    EXPANSION_TBC       = 1,    /**< The Burning Crusade expansion */
    EXPANSION_WOTLK     = 2,    /**< Wrath of the Lich King expansion */
    EXPANSION_CATA      = 3     /**< Cataclysm expansion */
};

enum MenuOptions    // this also defines menu order
{
    MENU_SLOT_HOME = 0,         /**< home */
    MENU_SLOT_ACCOUNT,          /**< register/acc info */
    MENU_SLOT_PASSWORD,         /**< pass recovery/pass change */
    MENU_SLOT_SERVER_STATUS,    /**< server status */
    MENU_SLOT_SUPPORT,          /**< donate, vote */
    MENU_SLOT_LINKS,            /**< links */
    MENU_SLOT_TELEPORT,         /**< nothing/teleport */
    MENU_SLOT_IP_LOCK,          /**< nothing/ip lock */
    MENU_SLOT_BAN,              /**< nothing/baninfo/banlist */
    MENU_SLOT_MUTE,             /**< nothing/muteinfo/mutelist */

    MENU_SLOT_GM_PANEL,         /**< nothing/nothing/panel gm */
    MENU_SLOT_GM_TICKETS,       /**< nothing/nothing/panel gm - tickets */
    MENU_SLOT_GM_ONLINE,        /**< nothing/nothing/panel gm - online check */
    MENU_SLOT_GM_FACTION,       /**< nothing/nothing/panel gm - faction check */

    MENU_SLOT_LOGIN,            /**< login/logout */

    MENU_SLOT_ERROR,            /**< for errors (shouldn't be visible) */

    MENU_SLOT_COUNT
};

enum Texts
{
    // Menu buttons
    TXT_MENU_HOME                   = 0,
    TXT_MENU_LOGIN                  = 1,
    TXT_MENU_LOGOUT                 = 2,
    TXT_MENU_REGISTER               = 3,
    TXT_MENU_PASS_RECOVERY          = 4,
    TXT_MENU_PASS_CHANGE            = 5,
    TXT_MENU_ACC_INFO               = 6,
    TXT_MENU_SERVER_STATUS          = 7,
    TXT_MENU_DONATE                 = 8,
    TXT_MENU_VOTE                   = 9,
    TXT_MENU_LINKS                  = 10,
    TXT_MENU_TELEPORT               = 11,
    TXT_MENU_IP_BLOCK               = 12,
    TXT_MENU_BANINFO                = 13,
    TXT_MENU_BANLIST                = 14,
    TXT_MENU_MUTEINFO               = 15,
    TXT_MENU_MUTELIST               = 16,
    TXT_MENU_ERROR                  = 17,

    TXT_MENU_GM_PANEL               = 50,
    TXT_MENU_GM_TICKETS             = 51,
    TXT_MENU_GM_ONLINE              = 52,
    TXT_MENU_GM_FACTION             = 53,

    // Static labels
    TXT_LBL_CHAR_NAME               = 75,
    TXT_LBL_CHAR_LVL                = 76,
    TXT_LBL_CHAR_CLASS              = 77,
    TXT_LBL_CHAR_TOTAL_PLAYED       = 78,
    TXT_LBL_CHAR_LVL_PLAYED         = 79,

    TXT_LBL_ACC_CHARS               = 100,
    TXT_LBL_ACC_CREATE_DATE         = 101,
    TXT_LBL_ACC_LAST_LOGIN          = 102,
    TXT_LBL_ACC_LAST_IP             = 103,
    TXT_LBL_ACC_IP_LOCK             = 104,
    TXT_LBL_ACC_BAN                 = 105,
    TXT_LBL_ACC_ONLINE              = 106,
    TXT_LBL_ACC_VP                  = 107,
    TXT_LBL_ACC_EXPANSION           = 108,
    TXT_LBL_ACC_CLIENT_VERSION      = 109,
    TXT_LBL_ACC_MULTIACC            = 110,
    TXT_LBL_ACC_LAST_PASS_RECOVERY  = 111,
    TXT_LBL_ACC_LOGIN               = 112,
    TXT_LBL_ACC_MAIL                = 113,
    TXT_LBL_ACC_GG                  = 114,
    TXT_LBL_ACC_INFO                = 115,      // page title
    TXT_LBL_ACC_IP_LOCK_ON          = 116,      // ip lock is currently on
    TXT_LBL_ACC_IP_LOCK_OFF         = 117,      // ip lock is currently off
    TXT_LBL_ACC_IP_BAN              = 118,

    TXT_LBL_PASS_CHANGE             = 130,
    TXT_LBL_PASS_OLD                = 131,
    TXT_LBL_PASS_NEW                = 132,
    TXT_LBL_PASS_REPEAT             = 133,
    TXT_LBL_PASS_RECOVERY           = 134,
    TXT_LBL_PASS_MAIL               = 135,
    TXT_LBL_PASS_GG                 = 136,

    TXT_LBL_INSTANCE_OPEN           = 150,      // instance is open
    TXT_LBL_INSTANCE_CLOSED         = 151,      // instance is closed (min lvl > 70)

    TXT_LBL_REGISTER_RULES          = 155,      // info that user must accept server rules
    TXT_LBL_REGISTER_RULES_ACCEPT   = 156,      // checkbox text
    TXT_LBL_REGISTER_MAIN           = 157,      // registration page title

    TXT_LBL_BAN_LOGIN               = 160,
    TXT_LBL_BAN_FROM                = 161,
    TXT_LBL_BAN_TO                  = 162,
    TXT_LBL_BAN_BY                  = 163,
    TXT_LBL_BAN_REASON              = 164,
    TXT_LBL_BAN_ACTIVE              = 165,
    TXT_LBL_BAN_NOT_ACTIVE          = 166,
    TXT_LBL_BAN_YES                 = 167,
    TXT_LBL_BAN_NO                  = 168,

    // Button labels
    TXT_BTN_PASS_CHANGE             = 200,
    TXT_BTN_PASS_CLEAR              = 201,
    TXT_BTN_PASS_SEND               = 202,
    TXT_BTN_LOGIN                   = 203,
    TXT_BTN_REGISTER                = 204,
    TXT_BTN_TELEPORT                = 205,
    TXT_BTN_BANNED_ACC              = 206,
    TXT_BTN_BANNED_IP               = 207,

    // Other
    TXT_SITE_TITLE                  = 225,
    TXT_SITE_FOOTER                 = 226,
    TXT_SERVER_INFO                 = 227,
    TXT_TELEPORT                    = 228,
    TXT_IP_LOCK_ON                  = 229,
    TXT_IP_LOCK_OFF                 = 230,
    TXT_BAN                         = 231,
    TXT_INSTANCES                   = 232,
    TXT_EXPANSION_PRETBC            = 233,
    TXT_EXPANSION_TBC               = 234,
    TXT_EXPANSION_WOTLK             = 235,
    TXT_EXPANSION_CATACLYSM         = 236,
    TXT_CURRENT_IP                  = 237,
    TXT_IS_ONLINE                   = 238,
    TXT_IS_OFFLINE                  = 239,
    TXT_CURRENT_IP_BAN              = 240,
    TXT_REGISTRATION_MAIL           = 241,

    TXT_ERROR_WRONG_LOGIN_DATA      = 350
};

void SendMail(WString& from, WString& to, WString& msg);
WString GetExpansionName(SessionInfo * sess, int index);
WString GetLocale(int index);
int irand(int min, int max);

#endif // DEFINES_H_INCLUDED
