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

#include <cstdio>

#ifdef DEBUG
#include <iostream>
#endif

#include <Wt/WString>

typedef uint64_t    uint64;
typedef uint32_t    uint32;
typedef uint16_t    uint16;
typedef uint8_t     uint8;
typedef int64_t     int64;
typedef int32_t     int32;
typedef int16_t     int16;
typedef int8_t      int8;

#ifdef WIN32
#define sscanf      sscanf_s
#define vsnprintf   vsnprintf_s
#endif

using namespace Wt;

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
    LANG_JA,        /**< Japanese */
    LANG_RU,        /**< Russian */
    LANG_CZ,        /**< Czech */
    LANG_IT,        /**< Italian */
    LANG_DE,        /**< German */
    LANG_FR,        /**< French */
    LANG_ES,        /**<  */
    LANG_CS,        /**<  */
    LANG_PT,        /**<  */
    LANG_ZH,        /**<  */

    LANG_COUNT
};

///********************************************//**
// * \brief Contains ids for all account levels
// *
// ***********************************************/
//
//enum AccountLevel
//{
//    LVL_LOGGED_OUT  = -2,   /**< recently logged out player */
//    LVL_NOT_LOGGED  = -1,   /**< not logged yet or new player */
//    LVL_PLAYER      = 0,    /**< logged normal player with no special rights */
//    LVL_GM_TRIAL    = 1,    /**< Game Master level 1 (Trial/Moderator) */
//    LVL_GM_HELPER   = 2,    /**< Game Master level 2 (Game Master Helper) */
//    LVL_GM_HEAD     = 3,    /**< Game Master level 3 (Head/Root Game Master) */
//    LVL_ADM         = 4,    /**< Server Administrator */
//
//
//    LVL_NONE        = 999,  /**< not existing level */
//};

enum AccountPermissionMasks
{
    PERM_NONE           = 0x000000,

    PERM_PLAYER         = 0x000001,
    PERM_DEVELOPER      = 0x000002,

    PERM_GM_TRIAL       = 0x000100,
    PERM_GM_HELPER      = 0x000200,

    PERM_GM_HEAD        = 0x000800,

    PERM_ADM_NORM       = 0x001000,
    PERM_ADM_HEAD       = 0x002000,

    PERM_CONSOLE        = 0x800000,

    PERM_GMT            = PERM_GM_TRIAL | PERM_GM_HELPER | PERM_GM_HEAD,
    PERM_ADM            = PERM_GMT | PERM_ADM_NORM | PERM_ADM_HEAD,
    PERM_HIGH_GMT       = PERM_ADM | PERM_GM_HEAD,
    PERM_GMT_DEV        = PERM_GMT | PERM_DEVELOPER,
    PERM_HIGH_DEV       = PERM_HIGH_GMT | PERM_DEVELOPER,
    PERM_ALL            = PERM_PLAYER | PERM_GMT_DEV | PERM_ADM
};

/********************************************//**
 * \brief Contains ids for account states
 ***********************************************/

enum AccountState
{
    ACCOUNT_STATE_INACTIVE  = 0,
    ACCOUNT_STATE_ACTIVE    = 1,
    ACCOUNT_STATE_IP_LOCKED = 2,
    ACCOUNT_STATE_FROZEN    = 3
};

enum SessionState
{
    SESSION_STATE_LOGGED_OUT    = 0,
    SESSION_STATE_NOT_LOGGED    = 1,
    SESSION_STATE_LOGGED_IN     = 2
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

    SessionInfo() : sessionState(SESSION_STATE_NOT_LOGGED), login(""), accountId(0), password(""),
                    email(""), language(LANG_PL), permissions(PERM_NONE), accountState(ACCOUNT_STATE_INACTIVE),
                    expansion(0), supportPoints(0), accountFlags(0), banned(false), currentRealm(0) {}
    ~SessionInfo() {}

    SessionState sessionState;

    WString login;              /**< Login used to log on account. */
    uint64 accountId;           /**< Players account id. Is 0 if player isn't logged in. */
    WString password;           /**< Password used to log on account. */
    WString email;              /**< Email used to register account. */
    WString joinDate;           /**< Account registration date */
    WString lastIp;             /**< Last ip used while connecting to server. */
    WString sessionIp;          /**< Ip for current panel session. */
    Lang language;              /**< Chosen language which should be used to show informations in panel. */
    uint64 permissions;         /**< Account permissions. */
    AccountState accountState;  /**< Account state - inactive/active/ip locked/frozen */
    int expansion;              /**< Expansion enabled for this account. */
    uint32 supportPoints;       /**< Vote points count */
    uint64 accountFlags;        /**< Account custom flags (blizz xp rates for example) */
    bool banned;                /**< Account was banned while login? */
    int currentRealm;           /**< Current realm - not used yet */

    /********************************************//**
     * \brief Returns information if account have ip lock enabled.
     * \return information about ip lock
     ***********************************************/

    bool IsIPLocked()
    {
        return accountState == ACCOUNT_STATE_IP_LOCKED;
    }

    /********************************************//**
     * \brief Returns information if account have ip lock enabled.
     * \return information about ip lock
     ***********************************************/

    bool IsFrozen()
    {
        return accountState == ACCOUNT_STATE_FROZEN;
    }

    /********************************************//**
     * \brief Returns information if account have ip lock enabled.
     * \return information about ip lock
     ***********************************************/

    bool IsActive()
    {
        return accountState == ACCOUNT_STATE_ACTIVE;
    }

    /********************************************//**
     * \brief Returns information if session is logged in.
     * \return information about session logged in state
     ***********************************************/

    bool IsLoggedIn()
    {
        return sessionState == SESSION_STATE_LOGGED_IN;
    }

    /********************************************//**
     * \brief Checks permissions for given permission mask
     * \return information if session has proper permissions
     ***********************************************/

    bool HasPermissionMask(uint64 mask)
    {
        return permissions & mask;
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
        sessionState = SESSION_STATE_LOGGED_OUT;

        login = "";
        password = "";
        email = "";
        joinDate = "";
        lastIp = "";

        accountId = 0;
        permissions = PERM_NONE;
        accountState = ACCOUNT_STATE_INACTIVE;
        expansion = 0;
        supportPoints = 0;
        banned = false;
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
    MENU_SLOT_CHARACTERS,       /**< nothing/character informations */
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

/** Menu */
#define TXT_MENU_HOME                   "menu.home"                 /**< Home menu button */
#define TXT_MENU_LOGIN                  "menu.login"                /**< Login menu button */
#define TXT_MENU_LOGOUT                 "menu.logout"               /**< Logout menu button */
#define TXT_MENU_REGISTER               "menu.register"             /**< Register menu button */
#define TXT_MENU_ACC_INFO               "menu.account"              /**< Account info menu button */
#define TXT_MENU_PASS_RECOVERY          "menu.password.recovery"    /**< Password recovery menu button */
#define TXT_MENU_PASS_CHANGE            "menu.password.change"      /**< Password change menu button */
#define TXT_MENU_IP_BLOCK               "menu.locks"                /**< Account locks menu button */
#define TXT_MENU_CHARACTERS             "menu.characters"           /**< Character information menu button */
#define TXT_MENU_TELEPORT               "menu.teleport"             /**< Teleport/Unstack menu button */
#define TXT_MENU_SUPPORT                "menu.support"              /**< Support main */
#define TXT_MENU_DONATE                 "menu.support.donate"       /**< Donate menu button */
#define TXT_MENU_VOTE                   "menu.support.vote"         /**< Vote menu button */
#define TXT_MENU_SERVER_STATUS          "menu.serverstatus"         /**< Server status menu button */
#define TXT_MENU_LINKS                  "menu.links"                /**< Links menu button */
#define TXT_MENU_LICENCE                "menu.licence"              /**< Licence informations page */

#define TXT_MENU_GM_PANEL               "menu.gm.panel"             /**< Game Masters panel main page menu button */
#define TXT_MENU_GM_TICKETS             "menu.gm.tickets"           /**< Game Masters tickets (in panel) menu button*/
#define TXT_MENU_GM_ONLINE              "menu.gm.online"            /**< Game Masters online check (in panel) menu button */
#define TXT_MENU_GM_FACTION             "menu.gm.faction"           /**< Game Masters faction check (in panel) menu button */
#define TXT_MENU_GM_BANLIST             "menu.gm.ban.list"          /**< Ban list menu button */
#define TXT_MENU_GM_MUTELIST            "menu.gm.mute.list"         /**< Mute list menu button */

/** Character */
#define TXT_CHAR_NAME                   "character.name"            /**< Character name label */
#define TXT_CHAR_LVL                    "character.level"           /**< Character level label */
#define TXT_CHAR_CLASS                  "character.class"           /**< Character class label */
#define TXT_CHAR_PLAYED_TOTAL           "character.played.total"    /**< Character total played time label */
#define TXT_CHAR_PLAYED_LEVEL           "character.played.level"    /**< Character current level played time label*/
#define TXT_CHAR_PLAYED_FMT             "character.played.fmt"      /**< Format for character total and level played time */
#define TXT_CHAR_LIST                   "character.list"            /**< Character list/choose label */
#define TXT_CHAR_RACE                   "character.race"            /**< Character race label */
#define TXT_CHAR_TALENT_RESET_COST      "character.talent.reset.cost"   /**< Character last talent reset cost label */
#define TXT_CHAR_ACT_TAL_RESET_COST     "character.talent.reset.cost.actual"   /**< Character actual talent reset cost label */
#define TXT_CHAR_TALENT_RESET_TIME      "character.talent.reset.time"   /**< Character last talent reset time label */
#define TXT_CHAR_ONLINE                 "character.onlinestate"     /**< Character is online label */
#define TXT_CHAR_TAB_INFO               "character.tab.info"        /**< Character basic informations tab label */
#define TXT_CHAR_TAB_QUEST              "character.tab.quest"       /**< Character quest informations tab label */
#define TXT_CHAR_TAB_SPELL              "character.tab.spell"       /**< Character spell informations tab label */
#define TXT_CHAR_TAB_INVENTORY          "character.tab.inventory"   /**< Character inventory informations tab label */
#define TXT_CHAR_TAB_FRIENDS            "character.tab.fiends"      /**< Character friends informations tab label */
#define TXT_CHAR_TAB_MAIL               "character.tab.mails"       /**< Character mails preview tab label */
#define TXT_CHAR_DELETION_DATE          "character.deletion.date"   /**< Character deletion date label */

#define TXT_CHAR_RESTORED               "character.restored"        /**< Information that character was restored successfully */

#define TXT_RACE_HUMAN                  "character.race.human"      /**< Text for human race */
#define TXT_RACE_ORC                    "character.race.orc"        /**< Text for orc race */
#define TXT_RACE_DWARF                  "character.race.dwarf"      /**< Text for dwarf race */
#define TXT_RACE_NIGHT_ELF              "character.race.nightelf"   /**< Text for night elf race */
#define TXT_RACE_UNDEAD                 "character.race.undead"     /**< Text for undead race */
#define TXT_RACE_TAUREN                 "character.race.tauren"     /**< Text for tauren race */
#define TXT_RACE_GNOME                  "character.race.gnome"      /**< Text for gnome race */
#define TXT_RACE_TROLL                  "character.race.troll"      /**< Text for troll race */
#define TXT_RACE_BLOOD_ELF              "character.race.bloodelf"   /**< Text for blood elf race */
#define TXT_RACE_DRAENEI                "character.race.draenei"    /**< Text for draenei race */

#define TXT_CLASS_WARRIOR               "character.class.warrior"   /**< Text for warrior class */
#define TXT_CLASS_PALADIN               "character.class.paladin"   /**< Text for paladin class */
#define TXT_CLASS_HUNTER                "character.class.hunter"    /**< Text for hunter class */
#define TXT_CLASS_ROGUE                 "character.class.rogue"     /**< Text for rogue class */
#define TXT_CLASS_PRIEST                "character.class.priest"    /**< Text for priest class */
#define TXT_CLASS_SHAMAN                "character.class.shaman"    /**< Text for shaman class */
#define TXT_CLASS_MAGE                  "character.class.mage"      /**< Text for mage class */
#define TXT_CLASS_WARLOCK               "character.class.warlock"   /**< Text for warlock class */
#define TXT_CLASS_DRUID                 "character.class.druid"     /**< Text for druid class */

/** Account */
#define TXT_ACC_CREATE_DATE             "account.create.date"       /**< Account create time label*/
#define TXT_ACC_LAST_LOGIN              "account.last.login"        /**< Account last login time label */
#define TXT_ACC_LAST_IP                 "account.last.ip"           /**< Account last login ip (server) label */
#define TXT_ACC_LAST_PASS_RECOVERY      "account.last.passwordrecovery"  /**< Account last pass recovery time label */
#define TXT_ACC_LOCK                    "account.lock"              /**< Account ip lock label */
#define TXT_ACC_LOCK_IP_STATE           "account.lock.ip.state"      /**< Account ip lock is currently on label */
#define TXT_ACC_BAN                     "account.ban.account"       /**< Account ban label */
#define TXT_ACC_BAN_IP                  "account.ban.ip"            /**< Account ip ban label */
#define TXT_ACC_ONLINE                  "account.online"            /**< Account online label */
#define TXT_ACC_SUPPORT_POINTS          "account.support.points"    /**< Account vote points label */
#define TXT_ACC_EXPANSION               "account.expansion"         /**< Account expansion label */
#define TXT_ACC_CLIENT_VERSION          "account.client"            /**< Account client version label */
#define TXT_ACC_MULTIACC                "account.multiacc"          /**< Account multiacc (account have enabled multiacc with other account ?) label */
#define TXT_ACC_LOGIN                   "account.login"             /**< Account login label */
#define TXT_ACC_MAIL                    "account.mail"              /**< Account mail label */
#define TXT_ACC_IM_TYPE                 "account.im.type"           /**< Account IM type (communicator protocol) label */
#define TXT_ACC_IM_IDENTIFIER           "account.im.id"             /**< Account IM id label */
#define TXT_ACC_TAB_INFO                "account.tab.informations"  /**< Account basic information tab label */
#define TXT_ACC_TAB_BAN                 "account.tab.ban"           /**< Account ban information tab label */
#define TXT_ACC_TAB_MUTE                "account.tab.mute"          /**< Account mute information tab label */
#define TXT_ACC_TAB_TICKET              "account.tab.ticket"        /**< Account ticket information tab label */
#define TXT_ACC_TAB_ACTIVITY            "account.tab.activity"      /**< Account activity information tab label */
#define TXT_ACC_XP_RATE                 "account.xp.rate"           /**< Account XP rates. */

/** Password modifications */
#define TXT_PASS_OLD                    "password.old"              /**< Old password label */
#define TXT_PASS_NEW                    "password.new"              /**< New password label */
#define TXT_PASS_REPEAT                 "password.repeat"           /**< Reapeat password label */
#define TXT_PASS_MAIL                   "password.mail"             /**< Mail for password recovery label */
#define TXT_PASS_IM_ID                  "password.im.id"            /**< IM identificator for password recovery label */
#define TXT_PASS_CHANGE_COMPLETE        "password.change.complete"  /**< Information that password change was successfull */
#define TXT_PASS_RECOVERY_MAIL          "password.recovery.mail"    /**< Email text for password recovery mails */
#define TXT_PASS_RECOVERY_SUBJECT       "password.recovery.mail.subject"    /**< Email subject for password recovery mails */
#define TXT_PASS_RECOVERY_COMPLETE      "password.recovery.complete"    /**< Information that password recovery was successfull */

/** Instance informations */
#define TXT_INSTANCE_OPEN               "instance.open"             /**< Instance is open label */
#define TXT_INSTANCE_CLOSED             "instance.closed"           /**< Instance is closed label (min lvl > 70) */

/** Registration */
#define TXT_REG_RULES                   "registration.rules"        /**< Label with info that user must accept server rules (for registration) */
#define TXT_REG_RULES_NOT_ACCEPTED      "registration.rules.notaccepted"  /**< Rules not accepted but button clicked */
#define TXT_REG_ACC_EXISTS              "registration.exists"       /**< There are already account with that login */
#define TXT_REG_MAIL                    "registration.mail"         /**< Email text for registration mails */
#define TXT_REG_SUBJECT                 "registration.mail.subject" /**< Email subject for registration mails */
#define TXT_REG_COMPLETE                "registration.complete"     /**< Information that registration was successfull */
#define TXT_REG_ERROR                   "registration.error"        /**< Information that there was an error in registration. */

/** Ban informations */
#define TXT_PUNISHMENT_FROM             "punishment.date.from"      /**< Punishment time label */
#define TXT_PUNISHMENT_TO               "punishment.date.to"        /**< Expiration time label */
#define TXT_PUNISHMENT_BY               "punishment.by"             /**< Punished by label */
#define TXT_PUNISHMENT_REASON           "punishment.reason"         /**< Reason label */
#define TXT_PUNISHMENT_TYPE             "punishment.type"           /**< Punishment type label */
#define TXT_PUNISHMENT_PERMANENT        "punishment.permanent"      /**< Panishment is permanent label */
#define TXT_PUNISHMENT_BANNED           "punishment.banned"         /**< Info that account is banned till xxx */

/** Mute informations */
#define TXT_MUTE_FROM                   "mute.date.from"            /**< Ban time label */
#define TXT_MUTE_TO                     "mute.date.to"              /**< Unban time label */
#define TXT_MUTE_BY                     "mute.by"                   /**< Banned by label */
#define TXT_MUTE_REASON                 "mute.reason"               /**< Ban reason label */

/** Server status */
#define TXT_STATUS_REALM                "status.realm"              /**< Realm label. */
#define TXT_STATUS_STATE                "status.state"              /**< Realm state label. */
#define TXT_STATUS_ONLINE               "status.online"             /**< Online players count label. */
#define TXT_STATUS_MAXONLINE            "status.online.max"         /**< Max online players count label. */
#define TXT_STATUS_FACTIONS             "status.factions"           /**< Factions label */
#define TXT_STATUS_FACTIONS_FMT         "status.factions.fmt"       /**< Format for displaing online horde and ally % */
#define TXT_STATUS_UPTIME               "status.uptime"             /**< Realm uptime label. */
#define TXT_STATUS_UPTIME_FMT           "status.uptime.fmt"         /**< format for server uptime time */
#define TXT_STATUS_REV                  "status.revision"           /**< Realm revision label. */
#define TXT_STATUS_DIFF                 "status.diff"               /**< Realm diff label. */
#define TXT_STATUS_AVGDIFF              "status.diff.avarage"       /**< Realm avarage diff label. */

/** Buttons */
#define TXT_BTN_PASS_CHANGE             "button.password.change"    /**< Password change button label */
#define TXT_BTN_PASS_CLEAR              "button.password.clear"     /**< Password clear button label */
#define TXT_BTN_PASS_SEND               "button.password.send"      /**< Send new password button label */
#define TXT_BTN_LOGIN                   "button.login"              /**< Log in to panel button label */
#define TXT_BTN_LOGOUT                  "button.logout"             /**< Log out from panel button label */
#define TXT_BTN_REGISTER                "button.register"           /**< Register button label */
#define TXT_BTN_TELEPORT                "button.teleport"           /**< Character teleport button label */
#define TXT_BTN_CHARACTER_RESTORE       "button.character.restore"  /**< Button to restore deleted character */
#define TXT_BTN_CHANGE_TEMPLATE         "button.template.change"    /**< Button to change panel template */

/** Spells */
#define TXT_SPELL_ID                    "spell.id"                  /**< Spell id label. */
#define TXT_SPELL_NAME                  "spell.name"                /**< Spell name label. */
#define TXT_SPELL_ACTIVE                "spell.active"              /**< Spell active info label. */
#define TXT_SPELL_DISABLED              "spell.disabled"            /**< Spell disabled info label. */

/** Items */
#define TXT_ITEM_ID                     "item.id"                   /**< Item id label. */
#define TXT_ITEM_NAME                   "item.name"                 /**< Item name label. */
#define TXT_ITEM_COUNT                  "item.count"                /**< Item count label. */

/** Quests */
#define TXT_QUEST_LINK_NAME_FMT         "quest.link.name.fmt"       /**< Format for link to hgdb with quest name as text */
#define TXT_QUEST_TOOLTIP_FMT           "quest.tooltip.fmt"         /**< Format for quest tooltip */
#define TXT_QUEST_NAME                  "quest.name"                /**< Quest name label. */
#define TXT_QUEST_LVL                   "quest.level"               /**< Quest level label. */
#define TXT_QUEST_STATUS                "quest.status"              /**< Quest status label. */

#define TXT_QUEST_STATUS_NONE           "quest.status.none"         /**< Quest isn't shown in quest list (default) */
#define TXT_QUEST_STATUS_COMPLETE       "quest.status.complete"     /**< Quest has been completed  */
#define TXT_QUEST_STATUS_UNAVAILABLE    "quest.status.unavailable"  /**< Quest is unavailable to the character */
#define TXT_QUEST_STATUS_INCOMPLETE     "quest.status.incomplete"   /**< Quest is active in quest log but incomplete */
#define TXT_QUEST_STATUS_AVAILABLE      "quest.status.available"    /**< Quest is available to be taken by character */
#define TXT_QUEST_STATUS_REWARDED       "quest.status.rewarded"     /**< Quest has been rewarded */

/** Friends */
#define TXT_FRIEND_NAME                 "friend.name"               /**< Friend name label. */
#define TXT_FRIEND_NOTE                 "friend.note"               /**< Friend note label. */

/** Mails */
#define TXT_MAIL_FROM                   "mail.from"                 /**< Mail from label. */
#define TXT_MAIL_SUBJECT                "mail.subject"              /**< Mail subject label. */
#define TXT_MAIL_DATE                   "mail.date"                 /**< Mail delivery date label. */
#define TXT_MAIL_EXPIRES                "mail.expires"              /**< Mail expire date label. */
#define TXT_MAIL_READED                 "mail.readed"               /**< Mail readed label. */
#define TXT_MAIL_PREVIEW_FMT            "mail.preview.fmt"          /**< Mail preview display format. */
#define TXT_MAIL_FROM_SUPPORT           "mail.from.support"         /**< Mail from support label. <-- predefinied from clause */
#define TXT_MAIL_FROM_AUCTION           "mail.from.auction"         /**< Mail from auction house label. <-- predefined from clause */

/** Site */
#define TXT_SITE_TITLE                  "site.title"                /**< Players panel site title */
#define TXT_SITE_FOOTER                 "site.footer"               /**< Players panel site footer */

/** Informations/main pages */
#define TXT_INFO_SERVER                 "informations.server"       /**< Static server informations (machine, rates etc) */
#define TXT_INFO_TELEPORT               "informations.teleport"     /**< Character teleport page informations */
#define TXT_INFO_INSTANCES              "informations.instances"    /**< Instances information page informations */
#define TXT_INFO_LOGOUT                 "informations.logout"       /**< info to show on logout page */
#define TXT_INFO_STATUS                 "informations.status"       /**< Server status page title. */
#define TXT_INFO_REGISTRATION           "informations.registration" /**< Account registration page title */
#define TXT_INFO_PASS_RECOVERY          "informations.password.recovery"    /**< Password recovery page title */
#define TXT_INFO_PASS_CHANGE            "informations.password.change"      /**< Password change page title */
#define TXT_INFO_ACCOUNT                "informations.account"      /**< Account informations page title */
#define TXT_INFO_SUPPORT                "informations.support"      /**< Support informations text */
#define TXT_INFO_SUPPORT_VOTE           "informations.support.vote" /**< Vote event informations text */
#define TXT_INFO_CHARACTER              "informations.character"    /**< Text for character page title/info */

/** Expansion names */
#define TXT_EXPANSION_CLASSIC           "expansion.classic"         /**< text/name for preTBC expansion */
#define TXT_EXPANSION_TBC               "expansion.tbc"             /**< text/name for TBC expansion  */
#define TXT_EXPANSION_WOTLK             "expansion.wotlk"           /**< text/name for WOTLK expansion */
#define TXT_EXPANSION_CATACLYSM         "expansion.cataclysm"       /**< text/name for Cata expansion */

/** General info */
#define TXT_GEN_ONLINE                  "general.online"            /**< General online label */
#define TXT_GEN_OFFLINE                 "general.offline"           /**< General offline label */
#define TXT_GEN_UNKNOWN                 "general.unknown"           /**< Text for unknown race/class/etc */
#define TXT_GEN_YES                     "general.yes"               /**< Text for 'yes' labels */
#define TXT_GEN_NO                      "general.no"                /**< Text for 'no' labels */
#define TXT_GEN_BANNED                  "general.banned"            /**< 'banned' informations */
#define TXT_GEN_NOT_BANNED              "general.notbanned"         /**< not banned info */
#define TXT_GEN_NEVER_BANNED            "general.never.banned"      /**< Information that player wasn't banned */
#define TXT_GEN_NEVER_MUTED             "general.never.muted"       /**< Information that player wasn't muted */
#define TXT_GEN_ACTIVE                  "general.active"            /**< General active label */
#define TXT_GEN_NOT_ACTIVE              "general.notactive"         /**< General not active label */
#define TXT_GEN_PERM                    "general.permanent"         /**< General permanent label */
#define TXT_GEN_TIMED                   "general.timed"             /**< General timed (not perm) label */
#define TXT_GEN_ON                      "general.on"                /**< General on label */
#define TXT_GEN_OFF                     "general.off"               /**< General off label */
#define TXT_GEN_NAME                    "general.name"              /**< General name label */
#define TXT_GEN_STATE                   "general.state"             /**< Genaral state label */
#define TXT_GEN_ERROR                   "general.error"             /**< Genaral error label */
#define TXT_GEN_LOADING                 "general.loading"           /**< General loading label */

/** Current ip */
#define TXT_CURRENT_IP                  "ip.current"                /**< Current ip label */
#define TXT_CURRENT_IP_BAN              "ip.current.ban"            /**< Current ip ban label */

/** Teleport option */
#define TXT_TELEPORT_SUCCESS            "teleport.success"          /**< Information that character teleportation was successfull */

/** Activity */
#define TXT_ACT_DATE                    "activity.date"             /**< Activity date label */
#define TXT_ACT_IP                      "activity.ip"               /**< Activity ip label */
#define TXT_ACT_TEXT                    "activity.text"             /**< Activity text label */
#define TXT_ACT_CHARACTER_RESTORE       "activity.character.restore"    /**< Activity log info: Character with %s restored */
#define TXT_ACT_IP_LOCK                 "activity.lock.change"      /**< Activity log info: Someone tried to change ip lock state */
#define TXT_ACT_LOGIN_SUCCESS           "activity.login.success"    /**< Activity log info: Account login successfull */
#define TXT_ACT_LOGIN_FAIL              "activity.login.fail"       /**< Activity log info: Account login fail */
#define TXT_ACT_PASS_CHANGE             "activity.password.change"  /**< Activity log info: Someone tried to change password */
#define TXT_ACT_RECOVERY_SUCCESS        "activity.recovery.success" /**< Activity log info: Password recovery success */
#define TXT_ACT_RECOVERY_FAIL           "activity.recovery.fail"    /**< Activity log info: Password recovery fail */
#define TXT_ACT_REGISTRATION_COMPLETE   "activity.registration.complete"    /**< Activity log info: Account registration complete */
#define TXT_ACT_XP_RATES                "activity.xp.rate.change"   /**< Activity log info: Someone tried to change XP rates for account */

/** Licence */
#define TXT_LICENCE_INTRO               "licence.intro"             /**< Static licence intro */
#define TXT_LICENCE_REPO                "licence.repository"        /**< Static link to panel repository/source code */
#define TXT_LICENCE_INFO                "licence.informations"      /**< Additional static informations */

/** Support */
#define TXT_SUPPORT_VOTE_READY          "support.vote.ready"        /**< Text for ready to vote */
#define TXT_SUPPORT_VOTE_NEXT           "support.vote.next"         /**< Text for next vote time info */
#define TXT_SUPPORT_VOTED               "support.vote.success"      /**< Text for successfull vote */

/** XP modes */
#define TXT_XP_RATE_SERVER              "xp.rate.default"           /**< Text for server default XP rates */
#define TXT_XP_RATE_BLIZZLIKE           "xp.rate.blizzlike"         /**< Text for blizzlike XP rates */

/** Template chooser */
#define TXT_TEMPLATE_CHOOSE             "template.choose"           /**< Text for chosing template */

/** Errors */
#define TXT_ERROR_WRONG_LOGIN_DATA      "error.wrong.data.login"    /**< Error info: wrong login or password */
#define TXT_ERROR_WRONG_RECOVERY_DATA   "error.wrong.data.recovery" /**< Error info: wrong login or email */
#define TXT_ERROR_WRONG_PASSWORD        "error.wrong.current.password"  /**< Error info: wrong current password */
#define TXT_ERROR_PASSWORDS_MISMATCH    "error.password.mismatch"   /**< Error info: typed passwords must be the same */
#define TXT_ERROR_PASSWORD_TO_SHORT     "error.password.toshort"    /**< Error info: password to short */
#define TXT_ERROR_PASSWORD_TO_LONG      "error.password.tolong"     /**< Error info: password to long */
#define TXT_ERROR_VALIDATION_LOGIN      "error.validation.login"    /**< Error info: Validate error: wrong data */
#define TXT_ERROR_VALIDATION_RECOVERY   "error.validation.recovery" /**< Error info: Validate error: wrong data */
#define TXT_ERROR_CANT_TELEPORT_ONLINE  "error.teleport.online"     /**< Error info: Only offline characters can be teleported */
#define TXT_ERROR_CHARACTER_NOT_FOUND   "error.character.notfound"  /**< Error info: character not found */
#define TXT_ERROR_IP_MISMATCH           "error.ip.mistach"          /**< Error info: IP lock on and IP is not matching */
#define TXT_ERROR_CANT_VOTE_TWICE       "error.vote.multiple"       /**< Error info: can't vote twice on same page */
#define TXT_ERROR_NEED_JAVA_SCRIPT      "error.javascript.required" /**< Error info: java script required */
#define TXT_ERROR_CHARACTER_NAME_EXISTS "error.character.name.exists"   /**< Error info: character with that name already exists */
#define TXT_ERROR_FACTION_MISMATCH      "error.faction.mismatch"    /**< Error info: you can't have characters in both sides */
#define TXT_ERROR_TO_MUCH_CHARACTERS    "error.tomuch.characters"   /**< Error info: to much characters on account */
#define TXT_ERROR_NOT_WHILE_BANNED      "error.cant.while.banned"   /**< Error info: you can't do that while banned */
#define TXT_ERROR_ACCOUNT_INACTIVE      "error.account.inactive"    /**< Error info: You can't login to inactive account. */
#define TXT_ERROR_ACCOUNT_STATE_UNKNOWN "error.account.state.unknown"   /**< Error info: Your account state is unknown. Please contact to administrator. */
#define TXT_ERROR_CANT_WHILE_FROZEN     "error.cant.while.frozen"   /**< Error info: You can't do that while account is frozen. */

/** Database errors */
#define TXT_ERROR_DB_CANT_CONNECT       "error.db.connection"       /**< DB Error info: can't connect to database */
#define TXT_ERROR_DB_QUERY_EMPTY        "error.db.query.empty"      /**< DB Error info: result empty */
#define TXT_ERROR_DB_QUERY_ERROR        "error.db.query.error"      /**< DB Error info: query error */

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
 * \brief Represents simple character location.
 ***********************************************/

struct Location
{
    Location() : mapId(0), zone(0), posX(0.0f), posY(0.0f), posZ(0.0f) {}
    Location (const Location & p) : mapId(p.mapId), zone(p.zone), posX(p.posX), posY(p.posY), posZ(p.posZ) {}

    uint32 mapId;   /**< Map id */
    uint32 zone;    /**< Zone on map */
    float posX;     /**< X Position */
    float posY;     /**< Y Position */
    float posZ;     /**< Z Position */
};

/********************************************//**
 * \brief Enum for DB Query result.
 ***********************************************/

enum DBResult
{
    DB_RESULT_ERROR     = -1,   /**< Informs that there was an error while executing query */
    DB_RESULT_EMPTY     = 0     /**< Informs that DB returned empty result */
};

/********************************************//**
 * \brief Enum for races.
 ***********************************************/

enum CharacterRaces
{
    RACE_HUMAN      = 1,    /**< Character race: Human */
    RACE_ORC        = 2,    /**< Character race: Orc */
    RACE_DWARF      = 3,    /**< Character race: Dwarf */
    RACE_NIGHT_ELF  = 4,    /**< Character race: Night elf */
    RACE_UNDEAD     = 5,    /**< Character race: Undead */
    RACE_TAUREN     = 6,    /**< Character race: Tauren */
    RACE_GNOME      = 7,    /**< Character race: Gnome */
    RACE_TROLL      = 8,    /**< Character race: Troll */
    RACE_BLOOD_ELF  = 10,   /**< Character race: Blood elf */
    RACE_DRAENEI    = 11    /**< Character race: Draenei */
};

/********************************************//**
 * \brief Enum for classes.
 ***********************************************/

enum CharacterClasses
{
    CLASS_WARRIOR       = 1,    /**< Character class: Warrior */
    CLASS_PALADIN       = 2,    /**< Character class: Paladin */
    CLASS_HUNTER        = 3,    /**< Character class: Hunter */
    CLASS_ROGUE         = 4,    /**< Character class: Rogue */
    CLASS_PRIEST        = 5,    /**< Character class: Priest */
    CLASS_DEATH_KNIGHT  = 6,    /**< Character class: Death Knight */
    CLASS_SHAMAN        = 7,    /**< Character class: Shaman */
    CLASS_MAGE          = 8,    /**< Character class: Mage */
    CLASS_WARLOCK       = 9,    /**< Character class: Warlock */
    CLASS_DRUID         = 11    /**< Character class: Druid */
};

/********************************************//**
 * \brief Enum with conflict side informations.
 ***********************************************/

enum ConflictSide
{
    SIDE_NONE       = 0,    /**< Conflict side: None */
    SIDE_HORDE      = 1,    /**< Conflict side: Alliande */
    SIDE_ALLIANCE   = 2,    /**< Conflict side: Horde */

    SIDE_UNKNOWN    = 5     /**< Unknown conflict side */
};

/********************************************//**
 * \brief Represents single spell informations.
 ***********************************************/

struct SpellInfo
{
    SpellInfo():
        entry(0), name("")
    {

    }

    SpellInfo(uint32 entry, WString name) :
        entry(entry), name(name)
    {

    }

    uint32 entry;
    WString name;
};

/********************************************//**
 * \brief Enum for realm informations
 ***********************************************/

enum RealmInfo
{
    REALM_INFO_NAME         = 0,    /**< Realm name */
    REALM_INFO_STATUS_URL   = 1,    /**< Link to file with realm status */
    REALM_INFO_ADDITIONAL   = 2,    /**< Additional informations about realm */
    REALM_INFO_ID           = 3,    /**< Realm id from Accounts database */
    REALM_INFO_DB_HOST      = 4,    /**< Host to realm database */
    REALM_INFO_DB_USER      = 5,    /**< User to realm database */
    REALM_INFO_DB_PASSWORD  = 6,    /**< Password to realm database */
    REALM_INFO_DB_PORT      = 7,    /**< Port to realm database */
    REALM_INFO_DB_NAME      = 8,    /**< Name of realm database */

    REALM_INFO_COUNT
};

struct RealmInformations
{
    RealmInformations()
        : name(""), statusUrl(""), additionalInfo(""), realmId(0),
            dbHost(""), dbLogin(""), dbPass(""), dbPort(0), dbName("")
    {

    }

    RealmInformations(const RealmInformations & p)
        : name(p.name), statusUrl(p.statusUrl), additionalInfo(p.additionalInfo), realmId(p.realmId),
            dbHost(p.dbHost), dbLogin(p.dbLogin), dbPass(p.dbPass), dbPort(p.dbPort), dbName(p.dbName)
    {

    }

    std::string name;
    std::string statusUrl;
    std::string additionalInfo;
    uint32 realmId;
    std::string dbHost;
    std::string dbLogin;
    std::string dbPass;
    int dbPort;
    std::string dbName;
};

// enums/defines from core:

enum PunishmentTypes
{
    PUNISHMENT_MUTE     = 1,
    PUNISHMENT_BAN      = 2
};

enum TimeConstants
{
    MINUTE = 60,
    HOUR   = MINUTE * 60,
    DAY    = HOUR * 24,
    WEEK   = DAY * 7,
    MONTH  = DAY * 30,
    YEAR   = MONTH * 12,
    IN_MILISECONDS = 1000
};

enum SocialFlag
{
    SOCIAL_FLAG_FRIEND      = 0x01,
    SOCIAL_FLAG_IGNORED     = 0x02,
    SOCIAL_FLAG_MUTED       = 0x04                          // guessed
};

enum MoneyConstants
{
    BRONZE  = 1,
    SILVER  = BRONZE * 100,
    GOLD    = SILVER * 100
};

/**
 * The type of the mail.
 * A mail can have 5 Different Types.
 */
enum MailMessageType
{
    MAIL_NORMAL         = 0,
    MAIL_AUCTION        = 2,
    MAIL_CREATURE       = 3,                                /// client send CMSG_CREATURE_QUERY on this mailmessagetype
    MAIL_GAMEOBJECT     = 4,                                /// client send CMSG_GAMEOBJECT_QUERY on this mailmessagetype
    MAIL_ITEM           = 5,                                /// client send CMSG_ITEM_QUERY on this mailmessagetype
};

/**
 * A Mask representing the status of the mail.
 */
enum MailCheckMask
{
    MAIL_CHECK_MASK_NONE        = 0x00,                     /// Nothing.
    MAIL_CHECK_MASK_READ        = 0x01,                     /// This mail was read.
    MAIL_CHECK_MASK_RETURNED    = 0x02,                     /// This mail was returned. No allow return mail.
    MAIL_CHECK_MASK_COPIED      = 0x04,                     /// This mail was copied. No allow make item copy from mail text.
    MAIL_CHECK_MASK_COD_PAYMENT = 0x08,                     /// This mail is payable on delivery.
    MAIL_CHECK_MASK_HAS_BODY    = 0x10,                     /// This mail has body text.
};

/**
 * The different types of Stationaries that exist for mails.
 * They have been gathered from Stationery.dbc
 */
enum MailStationery
{
    MAIL_STATIONERY_UNKNOWN =  1,
    MAIL_STATIONERY_DEFAULT = 41,
    MAIL_STATIONERY_GM      = 61,
    MAIL_STATIONERY_AUCTION = 62,
    MAIL_STATIONERY_VAL     = 64,
    MAIL_STATIONERY_CHR     = 65,
};

/**
 * Answers contained in mails from auctionhouses.
 */
enum MailAuctionAnswers
{
    AUCTION_OUTBIDDED           = 0,
    AUCTION_WON                 = 1,
    AUCTION_SUCCESSFUL          = 2,
    AUCTION_EXPIRED             = 3,
    AUCTION_CANCELLED_TO_BIDDER = 4,
    AUCTION_CANCELED            = 5,
    AUCTION_SALE_PENDING        = 6
};

struct Item
{
    Item() : name(""), id(0), guid(0), stackCount(0) {}
    Item(const Item & it)
    {
        name = it.name;
        id = it.id;
        guid = it.guid;
        stackCount = it.stackCount;
    }

    Wt::WString name;
    uint32 id;
    uint32 guid;
    uint32 stackCount;
};

struct TemplateInfo
{
    TemplateInfo() : name(""), stylePath(""), tmpltPath(""), currentTemplate("") {}
    TemplateInfo(const char * name, const char * style, const char * tmplt)
        : name(name), stylePath(style), tmpltPath(tmplt), currentTemplate("") {}

    std::string GetFullStylePath()
    {
        return stylePath + "/style.css";
    }

    std::string GetFullTemplatePath()
    {
        return tmpltPath + "/" + name + ".tmplt";
    }

    std::string name;
    std::string stylePath;
    std::string tmpltPath;

    std::string currentTemplate;
};

#define QUEST_TYPE_DAILY    87

#define Wttr Wt::WString::tr

#endif // DEFINES_H_INCLUDED
