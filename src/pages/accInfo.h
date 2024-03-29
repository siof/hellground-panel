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
 * \addtogroup Pages
 * \{
 *
 * \addtogroup Accounts Account Management
 * Account informations contains code to represent player
 * account information page.
 * Account informations consists of mostly 2 files:
 * - accInfo.h
 * - accInfo.cpp
 * \{
 *
 * \file accInfo.h
 * This file contains th headers needed to show player account informations.
 *
 ***********************************************/

#ifndef ACCINFO_H_INCLUDED
#define ACCINFO_H_INCLUDED

#include <Wt/WContainerWidget>

#include "../defines.h"

/********************************************//**
 * \brief Slots for Basic Account Informations
 *
 * Also determines order on page.
 *
 ***********************************************/

enum AccountInfoSlot
{
    ACCINFO_SLOT_TYPE = 0,          /**< Text with account expansion name. */
    ACCINFO_SLOT_CURRENT_IP,        /**< Current (panel session) ip. */
    ACCINFO_SLOT_CREATE_DATE,       /**< Account create time. */
    ACCINFO_SLOT_LAST_LOGIN_DATE,   /**< Last login date. */
    ACCINFO_SLOT_LAST_LOGGED_IP,    /**< Last logged (in game) ip. */
    ACCINFO_SLOT_STATE,             /**< Account state */
    ACCINFO_SLOT_ONLINE,            /**< Account is actually online. */
    ACCINFO_SLOT_CLIENT_VERSION,    /**< Players client locale name. */
    ACCINFO_SLOT_EMAIL,             /**< Player account email address. */
    ACCINFO_SLOT_VOTE_POINTS,       /**< Vote points count. */
    ACCINFO_SLOT_XP_RATE,           /**< Blizz/Server default XP rates. */
    ACCINFO_SLOT_MULTIACC,          /**< Multiacc feature is enabled ? */
    ACCINFO_SLOT_ACC_BAN,           /**< Is account banned ? */
    ACCINFO_SLOT_LAST_IP_BAN,       /**< Is last logged (in game) ip banned ? */
    ACCINFO_SLOT_CURR_IP_BAN,       /**< Is current (panel session) ip banned ? */

    ACCINFO_SLOT_COUNT
};

/********************************************//**
 * \brief Slots for punishment informations.
 *
 * It's only for simple text update on lang change.
 *
 ***********************************************/

enum PunishInfoSlot
{
    ACCPUNISHINFO_SLOT_PUNISHMENTDATE   = 0,    /**< Account punishment date */
    ACCPUNISHINFO_SLOT_EXPIRATIONDATE,          /**< Account punishment expiration date (or text for permanent bans) */
    ACCPUNISHINFO_SLOT_PUNISHEDBY,              /**< GM Name who punished account */
    ACCPUNISHINFO_SLOT_REASON,                  /**< Account punishment reason */
    ACCPUNISHINFO_SLOT_TYPE,                    /**< Info about punishment type */
    ACCPUNISHINFO_SLOT_PERMANENT,               /**< Info if punishment is permanent */
    ACCPUNISHINFO_SLOT_ACTIVE,                  /**< Info if punishment is still active */

    ACCPUNISHINFO_SLOT_COUNT
};

/********************************************//**
 * \brief Account info page tab slots
 *
 * Indexes of tabs in account info page
 *
 ***********************************************/

enum AccInfoTabSlot
{
    ACCTAB_SLOT_BASIC       = 0,    /**< Tab with basic account informations */
    ACCTAB_SLOT_BANS        = 1,    /**< Tab with ban informations */
    ACCTAB_SLOT_TICKET      = 2,    /**< Tab with ticket history */
    ACCTAB_SLOT_ACTIVITY    = 3,    /**< Tab with account activity history */

    ACCTAB_SLOT_COUNT
};

/********************************************//**
 * \brief A class to represents Account Informations page
 *
 * This class is container for other widgets with account informations.
 * Content should be created and added only on page creation.
 * All informations in widgets and texts which depend on language
 * should be only updated after page creation.
 *
 ***********************************************/

class AccountInfoPage : public WContainerWidget
{
public:
    AccountInfoPage(SessionInfo * sess, WContainerWidget * parent = 0);
    ~AccountInfoPage();

    void refresh();
private:
    /// panel session informations
    SessionInfo * session;
    /// tab widget to group specific account informations
    WTabWidget * tabs;
    /// tabs was created or not?
    bool needCreation;
    /// account page additional info
    WText * accPageInfo;

    void UpdateInformations();

    // basic account info
    /// table with basic account iformations
    Wt::WTable * accountInfo;
    WContainerWidget * CreateAccountInfo();
    void UpdateAccountInfo(bool first = false);

    WTable * CreatePunishmentInfo();

    WTabWidget * activityTabs;
    WContainerWidget * CreateActivityInfo();

    void ClearPage();

    void ChangeIPLock();
    void ChangeXPRates();

    WString GetEmail();
};

#endif // ACCINFO_H_INCLUDED

/********************************************//**
 * \}
 * \}
 ***********************************************/
