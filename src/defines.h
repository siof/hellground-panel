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

/********************************************//**
 *
 * \file defines.h
 * This file contains:
 * - standard includes for most files
 * - globally used structures
 * - globally used enums
 * - functions which should be accessible from each place in code
 * - global variables (included from config.h)
 *
 ***********************************************/


#ifndef DEFINES_H_INCLUDED
#define DEFINES_H_INCLUDED

#include "config.h"

// standard includes for most files
#include <WApplication>
#include <WEnvironment>
#include <WLogger>
#include <WString>
#include <WBreak>
#include <WContainerWidget>
#include <WLineEdit>
#include <WPushButton>
#include <WText>
#include <WImage>
#include <WTable>
#include <WStackedWidget>
#include <WAnimation>

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

/********************************************//**
 * \brief Contains numbers represents all supported languages.
 *
 * Order should be the same as columns order in database.
 *
 ***********************************************/

enum Lang
{
    LANG_PL = 0,    /**< Polish */
    LANG_EN,        /**< English */

    LANG_COUNT
};

/********************************************//**
 * \brief Contains ids for all account levels
 *
 ***********************************************/

enum AccountLevel
{
    LVL_LOGGED_OUT  = -2,   /**< recently logged out player */
    LVL_NOT_LOGGED  = -1,   /**< not logged yet or new player */
    LVL_PLAYER      = 0,    /**< logged normal player with no special rights */
    LVL_GM_TRIAL    = 1,    /**< Game Master level 1 (Trial/Moderator) */
    LVL_GM_HELPER   = 2,    /**< Game Master level 2 (Game Master Helper) */
    LVL_GM_HEAD     = 3,    /**< Game Master level 3 (Head/Root Game Master) */
    LVL_ADM         = 4     /**< Server Administrator */
};

#define ACCOUNT_LEVEL_COUNT 6

/********************************************//**
 * \brief Represents localized text loaded from DB
 *
 *
 * Contains single text with unique id for each supported language.
 *
 ***********************************************/

struct LangText
{
    LangText() : textId(0) {}
    LangText(uint32 id) : textId(id) {}
    ~LangText() {}

    WString& GetText(Lang lang)
    {
        return texts[lang];
    }

    uint32 textId;
    WString texts[LANG_COUNT];
};

/********************************************//**
 * \brief Contains panel session informations.
 *
 * Each player (logged or not) has his own session informations.
 * Most actions are based on informations stored in this struct.
 *
 ***********************************************/

struct SessionInfo
{
    /********************************************//**
     * \brief Creates new SessionInfo object.
     *
     ***********************************************/

    SessionInfo() : login(""), accid(0), pass(""), email(""), language(LANG_PL), accLvl(LVL_NOT_LOGGED), locked(false), expansion(0), vote(0), account_flags(0), textMissing("Error ! Text missing !") {}
    ~SessionInfo() {}

    WString login;          /**< Login used to log on account. */
    uint64 accid;           /**< Players account id. Is 0 if player isn't logged in. */
    WString pass;           /**< Password used to log on account. */
    WString email;          /**< Email used to register account. */
    WString joinDate;       /**< Account registration date */
    WString lastIp;         /**< Last ip used while connecting to server. */
    WString sessionIp;      /**< Ip for current panel session. */
    Lang language;          /**< Chosen language which should be used to show informations in panel. */
    AccountLevel accLvl;    /**< Security level for this account(player, gm etc). Which content should be shown depends on this variable (For example: Player can't use GM section. Not logged player can't view account informations.). */
    std::map<uint32 ,LangText> langTexts;   /**< Map with texts depend for supported languages loaded from database */
    bool locked;            /**< IP lock */
    int expansion;          /**< Expansion enabled for this account. */
    uint32 vote;            /**< Vote points count */
    uint64 account_flags;   /**< Account custom flags (blizz xp rates for example) */

    WString textMissing;    /**< "Error ! Text missing !" info. Should be shown when text wasn't found in langTexts. */

    /********************************************//**
     * \brief Returns text based on ID and current language.
     *
     * \param id    Text id which should be returned.
     * \return a referrence to text.
     *
     ***********************************************/

    WString& GetText(uint32 id)
    {
        std::map<uint32, LangText>::iterator itr = langTexts.find(id);

        if (itr == langTexts.end())
            return textMissing;

        return itr->second.GetText(language);
    }

    /********************************************//**
     * \brief Checks if there is text based on given ID.
     *
     * \param id    Text id which should be returned.
     * \return information that text exists or not.
     *
     ***********************************************/

    bool HasText(uint32 id)
    {
        std::map<uint32, LangText>::iterator itr = langTexts.find(id);

        return itr != langTexts.end();
    }

    /********************************************//**
     * \brief Clears session informations.
     *
     * Function to clear informations in session object.
     * Should be called only in logout case.
     *
     ***********************************************/

    void Clear()
    {
        login = "";
        pass = "";
        email = "";
        joinDate = "";
        lastIp = "";

        accid = 0;
        accLvl = LVL_LOGGED_OUT;
        locked = false;
        expansion = 0;
        vote = 0;
    }
};

/********************************************//**
 * \brief Slots for text visible on Error page.
 *
 * This also defines order (if there will be errors in more than 1 slot)
 *
 ***********************************************/

enum ErrorSlots
{
    ERROR_SLOT_BASE     = 0,    /**< Base slot - for basic errors */
    ERROR_SLOT_ADDITIONAL,      /**< Additional slot - if we want to add some info to base error */
    ERROR_SLOT_DB,              /**< Database slot - for database errors */
    ERROR_SLOT_INFO,            /**< Informations slot - for additional informations to display */
    ERROR_SLOT_COUNT            /**< Slots count */
};

/********************************************//**
 * \brief World of Warcraft expansions.
 *
 ***********************************************/

enum Expansion
{
    EXPANSION_PRETBC    = 0,    /**< Vanilla WoW (pre TBC, without expansion) */
    EXPANSION_TBC       = 1,    /**< The Burning Crusade expansion */
    EXPANSION_WOTLK     = 2,    /**< Wrath of the Lich King expansion */
    EXPANSION_CATA      = 3     /**< Cataclysm expansion */
};

/********************************************//**
 * \brief Menu slots for menu options.
 *
 * In 1 slot can be options for all account levels.
 * This also define menu order.
 *
 ***********************************************/

enum MenuOptions
{
    MENU_SLOT_HOME = 0,         /**< home */
    MENU_SLOT_ACCOUNT,          /**< register/acc info */
    MENU_SLOT_PASSWORD,         /**< pass recovery/pass change */
    MENU_SLOT_TELEPORT,         /**< nothing/teleport */
    MENU_SLOT_SUPPORT,          /**< donate, vote */
    MENU_SLOT_LINKS,            /**< links */
    MENU_SLOT_SERVER_STATUS,    /**< server status */

    MENU_SLOT_GM_PANEL,         /**< nothing/nothing/panel gm */
    MENU_SLOT_GM_TICKETS,       /**< nothing/nothing/panel gm - tickets */
    MENU_SLOT_GM_ONLINE,        /**< nothing/nothing/panel gm - online check */
    MENU_SLOT_GM_FACTION,       /**< nothing/nothing/panel gm - faction check */

    MENU_SLOT_LOGIN,            /**< login/logout */

    MENU_SLOT_LICENCE,          /**< licence and panel informations */

    MENU_SLOT_ERROR,            /**< for errors (shouldn't be visible) */

    MENU_SLOT_COUNT             /**< menu slots count */
};

/********************************************//**
 * \brief Language depend text ids.
 *
 * Ids must be the same as in database.
 *
 ***********************************************/

enum Texts
{
    /** Buttons */
    TXT_MENU_HOME                   = 0,    /**< Home menu button */
    TXT_MENU_LOGIN                  = 1,    /**< Login menu button */
    TXT_MENU_LOGOUT                 = 2,    /**< Logout menu button */
    TXT_MENU_REGISTER               = 3,    /**< Register menu button */
    TXT_MENU_PASS_RECOVERY          = 4,    /**< Password recovery menu button */
    TXT_MENU_PASS_CHANGE            = 5,    /**< Password change menu button */
    TXT_MENU_ACC_INFO               = 6,    /**< Account info menu button */
    TXT_MENU_SERVER_STATUS          = 7,    /**< Server status menu button */
    TXT_MENU_DONATE                 = 8,    /**< Donate menu button */
    TXT_MENU_VOTE                   = 9,    /**< Vote menu button */
    TXT_MENU_LINKS                  = 10,   /**< Links menu button */
    TXT_MENU_TELEPORT               = 11,   /**< Teleport/Unstack menu button */
    TXT_MENU_IP_BLOCK               = 12,   /**< Ip lock menu button */
    TXT_MENU_BANINFO                = 13,   /**< Ban informations menu button */
    TXT_MENU_BANLIST                = 14,   /**< Ban list menu button */
    TXT_MENU_MUTEINFO               = 15,   /**< Mute informations menu button */
    TXT_MENU_MUTELIST               = 16,   /**< Mute list menu button */
    TXT_MENU_ERROR                  = 17,   /**< Error page (shouldn't be visible) */
    TXT_MENU_LICENCE                = 18,   /**< Licence informations page */
    TXT_MENU_SUPPORT                = 19,   /**< Support main */

    TXT_MENU_GM_PANEL               = 50,   /**< Game Masters panel menu button */
    TXT_MENU_GM_TICKETS             = 51,   /**< Game Masters tickets (in panel) menu button*/
    TXT_MENU_GM_ONLINE              = 52,   /**< Game Masters online check (in panel) menu button */
    TXT_MENU_GM_FACTION             = 53,   /**< Game Masters faction check (in panel) menu button */

    /** Static texts/labels */
    /** Character */
    TXT_LBL_CHAR_NAME               = 75,   /**< Character name label */
    TXT_LBL_CHAR_LVL                = 76,   /**< Character level label */
    TXT_LBL_CHAR_CLASS              = 77,   /**< Character class label */
    TXT_LBL_CHAR_TOTAL_PLAYED       = 78,   /**< Character total played time label */
    TXT_LBL_CHAR_LVL_PLAYED         = 79,   /**< Character current level played time label*/

    /** Account */
    TXT_LBL_ACC_CHARS               = 100,  /**< Account characters label */
    TXT_LBL_ACC_CREATE_DATE         = 101,  /**< Account create time label*/
    TXT_LBL_ACC_LAST_LOGIN          = 102,  /**< Account last login time label */
    TXT_LBL_ACC_LAST_IP             = 103,  /**< Account last login ip (server) label */
    TXT_LBL_ACC_IP_LOCK             = 104,  /**< Account ip lock label */
    TXT_LBL_ACC_BAN                 = 105,  /**< Account ban label */
    TXT_LBL_ACC_ONLINE              = 106,  /**< Account online label */
    TXT_LBL_ACC_VP                  = 107,  /**< Account vote points label */
    TXT_LBL_ACC_EXPANSION           = 108,  /**< Account expansion label */
    TXT_LBL_ACC_CLIENT_VERSION      = 109,  /**< Account client version label */
    TXT_LBL_ACC_MULTIACC            = 110,  /**< Account multiacc (account have enabled multiacc with other account ?) label */
    TXT_LBL_ACC_LAST_PASS_RECOVERY  = 111,  /**< Account last pass recovery time label */
    TXT_LBL_ACC_LOGIN               = 112,  /**< Account login label */
    TXT_LBL_ACC_MAIL                = 113,  /**< Account mail label */
    TXT_LBL_ACC_GG                  = 114,  /**< Account IM label */
    TXT_LBL_ACC_INFO                = 115,  /**< Account informations page title */
    TXT_LBL_ACC_IP_LOCK_ON          = 116,  /**< Account ip lock is currently on label */
    TXT_LBL_ACC_IP_LOCK_OFF         = 117,  /**< Account ip lock is currently off label */
    TXT_LBL_ACC_IP_BAN              = 118,  /**< Account ip ban label */
    TXT_LBL_ACC_TAB_INFO            = 119,  /**< Account basic information tab label */
    TXT_LBL_ACC_TAB_BAN             = 120,  /**< Account ban information tab label */
    TXT_LBL_ACC_TAB_MUTE            = 121,  /**< Account mute information tab label */
    TXT_LBL_ACC_TAB_TICKET          = 122,  /**< Account ticket information tab label */
    TXT_LBL_ACC_TAB_ACTIVITY        = 123,  /**< Account activity information tab label */
    TXT_LBL_ACC_XP_RATES            = 124,  /**< Account XP rates. */

    /** Password modifications */
    TXT_LBL_PASS_CHANGE             = 130,  /**< Password change page title */
    TXT_LBL_PASS_OLD                = 131,  /**< Old password label */
    TXT_LBL_PASS_NEW                = 132,  /**< New password label */
    TXT_LBL_PASS_REPEAT             = 133,  /**< Reapeat password label */
    TXT_LBL_PASS_RECOVERY           = 134,  /**< Password recovery page title */
    TXT_LBL_PASS_MAIL               = 135,  /**< Mail for password recovery label */
    TXT_LBL_PASS_GG                 = 136,  /**< IM number for password recovery label */

    /** Instance informations */
    TXT_LBL_INSTANCE_OPEN           = 150,  /**< Instance is open label */
    TXT_LBL_INSTANCE_CLOSED         = 151,  /**< Instance is closed label (min lvl > 70) */

    /** Registration */
    TXT_LBL_REG_RULES               = 155,  /**< Label with info that user must accept server rules (for registration) */
    TXT_LBL_REG_RULES_ACCEPT        = 156,  /**< Accept server rules checkbox text */
    TXT_LBL_REG_MAIN                = 157,  /**< Account registration page title */
    TXT_LBL_REG_RULES_NOT_ACCEPTED  = 158,  /**< Rules not accepted but button clicked */
    TXT_LBL_REG_ACC_EXISTS          = 159,  /**< There are already account with that login */

    /** Ban informations */
    TXT_LBL_BAN_LOGIN               = 160,  /**< Banned account login label */
    TXT_LBL_BAN_FROM                = 161,  /**< Ban time label */
    TXT_LBL_BAN_TO                  = 162,  /**< Unban time label */
    TXT_LBL_BAN_BY                  = 163,  /**< Banned by label */
    TXT_LBL_BAN_REASON              = 164,  /**< Ban reason label */
    TXT_LBL_BAN_ACTIVE              = 165,  /**< Ban is active label */
    TXT_LBL_BAN_NOT_ACTIVE          = 166,  /**< Ban is inactive label */
    TXT_LBL_BAN_YES                 = 167,  /**< Account is banned label */
    TXT_LBL_BAN_NO                  = 168,  /**< Account isn't banned label */
    TXT_LBL_BAN_PERMANENT           = 169,  /**< Ban is permanent label */

    /** Mute informations */
    TXT_LBL_MUTE_FROM               = 170,  /**< Ban time label */
    TXT_LBL_MUTE_TO                 = 171,  /**< Unban time label */
    TXT_LBL_MUTE_BY                 = 172,  /**< Banned by label */
    TXT_LBL_MUTE_REASON             = 173,  /**< Ban reason label */
    TXT_LBL_MUTE_ACTIVE             = 174,  /**< Ban is active label */
    TXT_LBL_MUTE_NOT_ACTIVE         = 175,  /**< Ban is inactive label */
    TXT_LBL_MUTE_YES                = 176,  /**< Account is muted label */
    TXT_LBL_MUTE_NO                 = 177,  /**< Account isn't muted label */

    /** Server status */
    TXT_LBL_STATUS_REALM            = 185,  /**< Realm label. */
    TXT_LBL_STATUS_STATE            = 186,  /**< Realm state label. */
    TXT_LBL_STATUS_ONLINE           = 187,  /**< Online players count label. */
    TXT_LBL_STATUS_MAXONLINE        = 188,  /**< Max online players count label. */
    TXT_LBL_STATUS_FACTIONS_FMT     = 189,  /**< Format for displaing online horde and ally % */
    TXT_LBL_STATUS_UPTIME           = 190,  /**< Realm uptime label. */
    TXT_LBL_STATUS_REV              = 191,  /**< Realm revision label. */
    TXT_LBL_STATUS_DIFF             = 192,  /**< Realm diff label. */
    TXT_LBL_STATUS_AVGDIFF          = 193,  /**< Realm avarage diff label. */
    TXT_LBL_STATUS_INFO             = 194,  /**< Realm additional informations label. */
    TXT_LBL_STATUS_MAIN             = 195,  /**< Server status page title. */

    /** Button labels */
    TXT_BTN_PASS_CHANGE             = 200,  /**< Password change button label */
    TXT_BTN_PASS_CLEAR              = 201,  /**< Password clear button label */
    TXT_BTN_PASS_SEND               = 202,  /**< Send new password button label */
    TXT_BTN_LOGIN                   = 203,  /**< Log in to panel button label */
    TXT_BTN_REGISTER                = 204,  /**< Register button label */
    TXT_BTN_TELEPORT                = 205,  /**< Character teleport button label */
    TXT_BTN_BANNED_ACC              = 206,  /**< Show banned account button label */
    TXT_BTN_BANNED_IP               = 207,  /**< Show banned ips button label */
    TXT_BTN_LOGOUT                  = 208,  /**< Log out from panel button label */

    /** Other */
    TXT_SITE_TITLE                  = 225,  /**< Players panel site title */
    TXT_SITE_FOOTER                 = 226,  /**< Players panel site footer */
    TXT_SERVER_INFO                 = 227,  /**< Static server informations (machine, rates etc) */
    TXT_TELEPORT                    = 228,  /**< Character teleport page informations */
    TXT_IP_LOCK_ON                  = 229,  /**< IP lock is on info */
    TXT_IP_LOCK_OFF                 = 230,  /**< IP lock is off info */
    TXT_BAN                         = 231,  /**< Bans page title/static informations */
    TXT_INSTANCES                   = 232,  /**< Instances information page informations */
    TXT_EXPANSION_PRETBC            = 233,  /**< text/name for preTBC expansion */
    TXT_EXPANSION_TBC               = 234,  /**< text/name for TBC expansion  */
    TXT_EXPANSION_WOTLK             = 235,  /**< text/name for WOTLK expansion */
    TXT_EXPANSION_CATACLYSM         = 236,  /**< text/name for Cata expansion */
    TXT_CURRENT_IP                  = 237,  /**< Current ip label */
    TXT_IS_ONLINE                   = 238,  /**< Account is online label */
    TXT_IS_OFFLINE                  = 239,  /**< Account is offline label */
    TXT_CURRENT_IP_BAN              = 240,  /**< Current ip ban label */
    TXT_REGISTRATION_MAIL           = 241,  /**< Email text for registration mails */
    TXT_REGISTRATION_SUBJECT        = 242,  /**< Email subject for registration mails */
    TXT_RECOVERY_MAIL               = 243,  /**< Email text for password recovery mails */
    TXT_RECOVERY_SUBJECT            = 244,  /**< Email subject for password recovery mails */
    TXT_REGISTRATION_COMPLETE       = 245,  /**< Information that registration was successfull */
    TXT_RECOVERY_COMPLETE           = 246,  /**< Information that password recovery was successfull */
    TXT_PASS_CHANGE_COMPLETE        = 247,  /**< Information that password change was successfull */
    TXT_ONLINE                      = 248,  /**< guess ... */
    TXT_OFFLINE                     = 249,  /**< guess ... */
    TXT_UPTIME_FMT                  = 250,  /**< format for server uptime time */
    TXT_LOGOUT_INFO                 = 251,  /**< info to show on logout page */
    TXT_REGISTRATION_ERROR          = 252,  /**< Information that there was an error in registration. */
    TXT_TELEPORT_SUCCESSFULL        = 253,  /**< Information that character teleportation was successfull */
    TXT_NEVER_BANNED                = 254,  /**< Information that player wasn't banned */
    TXT_NEVER_MUTED                 = 255,  /**< Information that player wasn't muted */
    TXT_ACTIVITY_DATE               = 256,  /**< Activity date label */
    TXT_ACTIVITY_IP                 = 257,  /**< Activity ip label */
    TXT_ACTIVITY_TEXT               = 258,  /**< Activity text label */
    TXT_LICENCE_INTRO               = 259,  /**< Static licence intro */
    TXT_LICENCE_REPO                = 260,  /**< Static link to panel repository/source code */
    TXT_LICENCE_INFO                = 261,  /**< Additional static informations */
    TXT_SUPPORT_MAIN                = 262,  /**< Static text for main support page */
    TXT_SUPPORT_VOTE_INFO           = 263,  /**< Text for vote page title/info */
    TXT_SUPPORT_VOTE_NEXT           = 264,  /**< Text for next vote time info */
    TXT_SUPPORT_VOTED               = 265,  /**< Text for successfull vote */
    TXT_XP_RATE_SERVER              = 266,  /**< Text for server default XP rates */
    TXT_XP_RATE_BLIZZLIKE           = 267,  /**< Text for blizzlike XP rates */

    TXT_ERROR_WRONG_LOGIN_DATA      = 350,  /**< Error info: wrong login or password */
    TXT_ERROR_WRONG_RECOVERY_DATA   = 351,  /**< Error info: wrong login or email */
    TXT_ERROR_PASSWORDS_MISMATCH    = 352,  /**< Error info: typed passwords must be the same */
    TXT_ERROR_PASSWORD_TO_SHORT     = 353,  /**< Error info: password to short */
    TXT_ERROR_PASSWORD_TO_LONG      = 354,  /**< Error info: password to long */
    TXT_ERROR_NOT_VALID_DATA        = 355,  /**< Error info: Validate error: wrong data */
    TXT_ERROR_CANT_TELEPORT_ONLINE  = 356,  /**< Error info: Only offline characters can be teleported */
    TXT_ERROR_CHARACTER_NOT_FOUND   = 357,  /**< Error info: character not found */
    TXT_ERROR_IP_MISMATCH           = 358,  /**< Error info: IP lock on and IP is not matching */
    TXT_ERROR_WRONG_PASSWORD        = 359,  /**< Error info: wrong current password */
    TXT_ERROR_CANT_VOTE_TWICE       = 360,  /**< Error info: can't vote twice on same page */
    TXT_ERROR_NEED_JAVA_SCRIPT      = 361,  /**< Error info: java script required */

    TXT_DBERROR_CANT_CONNECT        = 501,  /**< DB Error info: can't connect to database */
    TXT_DBERROR_QUERY_EMPTY         = 502,  /**< DB Error info: result empty */
    TXT_DBERROR_QUERY_ERROR         = 503,  /**< DB Error info: query error */

    TXT_ACT_LOGIN_SUCCESS           = 601,  /**< Activity log info: Account login successfull */
    TXT_ACT_LOGIN_FAIL              = 602,  /**< Activity log info: Account login fail */
    TXT_ACT_REGISTRATION_COMPLETE   = 603,  /**< Activity log info: Account registration complete */
    TXT_ACT_PASS_CHANGE             = 604,  /**< Activity log info: Someone tried to change password */
    TXT_ACT_RECOVERY_SUCCESS        = 605,  /**< Activity log info: Password recovery success */
    TXT_ACT_RECOVERY_FAIL           = 606,  /**< Activity log info: Password recovery fail */
    TXT_ACT_IP_LOCK                 = 607,  /**< Activity log info: Someone tried to change ip lock state */
    TXT_ACT_XP_RATES                = 608,  /**< Activity log info: Someone tried to change XP rates for account */
};

/********************************************//**
 * \brief Sends email.
 *
 * \param from  source email address
 * \param to    destination email address
 * \param sub   subject for email
 * \param msg   message to send
 *
 * Function to send email from source email address to destination email address contains given message and subject.
 *
 ***********************************************/

extern void SendMail(WString& from, WString& to, WString& sub, WString& msg);

/********************************************//**
 * \brief Returns expansion name.
 *
 * \param sess      player session informations (needed to get expansion name from db)
 * \param index     expansion id
 * \return expansion name from db
 *
 * Returns expansion name for given index.
 * Name is stored in db and loaded on session creation so there is only need to read this information depends on index.
 *
 ***********************************************/

extern WString GetExpansionName(SessionInfo * sess, int index);

/********************************************//**
 * \brief Return client locale name.
 *
 * \param index     locale index
 * \return locale name
 *
 * Returns players client locale depends on locale index.
 *
 ***********************************************/

extern WString GetLocale(int index);

/********************************************//**
 * \brief Simple random function.
 *
 * \param min   minimum value
 * \param max   maximum value
 * \return randomized value beatween min and max
 *
 ***********************************************/

extern int irand(int min, int max);

#define SERVER_DB_DATA  SQL_HOST, SQL_LOGIN, SQL_PASSWORD, SQL_PORT
#define PANEL_DB_DATA   PANEL_SQL_HOST, PANEL_SQL_LOGIN, PANEL_SQL_PASS, PANEL_SQL_PORT

/********************************************//**
 * \brief Debug flags for console debug.
 ***********************************************/

enum DebugFlags
{
    DEBUG_NONE  = 0x00,
    DEBUG_DB    = 0x01,  /**< Debug flag for standard database debug informations */
    DEBUG_CODE  = 0x02   /**< Debug flag for standard code debug infornations */
};

/********************************************//**
 * \brief Debug flags for console debug.
 ***********************************************/

enum LogFlags
{
    LOG_NONE            = 0x00,
    LOG_DB_QUERY        = 0x01,     /**< Log flag for logging all DB queries */
    LOG_DB_ERRORS       = 0x02,     /**< Log flag for logging DB query errors */
    LOG_INVALID_DATA    = 0x04,     /**< Log flag for logging validation errors */
    LOG_STRANGE_DATA    = 0x08,     /**< Log flag for logging errors probably caused by strange data received from user */

    LOG_DB  = LOG_DB_QUERY | LOG_DB_ERRORS,
    LOG_ALL = LOG_DB | LOG_INVALID_DATA | LOG_STRANGE_DATA,
};

/********************************************//**
 * \brief Writes debug info to console
 *
 * \param flag  Debug flags for which debug should be printed to console.
 * \param text  Text format which should be printed to console.
 *
 * This function checks flags defined in config and decides
 * if debug should be printed to standard output or not.
 * If yes then function fills format with additional data and
 * sends to std out.
 *
 ***********************************************/

extern void console(DebugFlags flag, char const* text, ...);

/********************************************//**
 * \brief Handles logging
 *
 * \param flag  Log flags for which logging should be done.
 * \param text  Text format which should be logged.
 *
 * This function checks flags defined in config and decides
 * if logging should be done or not.
 * If yes then function fills format with additional data and
 * sends it to log.
 *
 ***********************************************/

extern void Log(LogFlags flag, char const* text, ...);

/********************************************//**
 * \brief Represents simple character location.
 ***********************************************/

struct Location
{
    uint32 mapId;   /**< Map id */
    uint32 zone;    /**< Zone on map */
    float posX;     /**< X Position */
    float posY;     /**< Y Position */
    float posZ;     /**< Z Position */
};

/********************************************//**
 * \brief Sets teleport position depends on race.
 *
 * \param race  Character race.
 * \param loc   Variable in which location will be stored.
 *
 ***********************************************/

extern void GetTeleportPosition(int race, Location & loc);

enum MysqlReturn
{
    RETURN_ERROR = -1,
    RETURN_EMPTY = 0
};

/********************************************//**
 * \brief Resturns format string filled with data.
 *
 * \param format    string format to fill
 * \return format string with filled with data
 *
 ***********************************************/

extern std::string GetFormattedString(const char * format, ...);

#endif // DEFINES_H_INCLUDED
