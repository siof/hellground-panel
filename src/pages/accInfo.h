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

#include "../defines.h"
#include "../slotItems.h"

/********************************************//**
 * \brief Slots for Account Informations page
 *
 * Also determines order on page.
 *
 ***********************************************/

enum AccountInfoSlot
{
    ACCINFO_SLOT_INFO   = 0,        /**< Page title text. */
    ACCINFO_SLOT_TYPE,              /**< Text with account expansion name. */
    ACCINFO_SLOT_CURRENT_IP,        /**< Current (panel session) ip. */
    ACCINFO_SLOT_CREATE_DATE,       /**< Account create time. */
    ACCINFO_SLOT_LAST_LOGIN_DATE,   /**< Last login date. */
    ACCINFO_SLOT_LAST_LOGGED_IP,    /**< Last logged (in game) ip. */
    ACCINFO_SLOT_IP_LOCK,           /**< Is ip lock on or not ? */
    ACCINFO_SLOT_ONLINE,            /**< Account is actually online. */
    ACCINFO_SLOT_CLIENT_VERSION,    /**< Players client locale name. */
    ACCINFO_SLOT_EMAIL,             /**< Player account email address. */
    ACCINFO_SLOT_VOTE_POINTS,       /**< Vote points count. */
    ACCINFO_SLOT_MULTIACC,          /**< Multiacc feature is enabled ? */
    ACCINFO_SLOT_ACC_BAN,           /**< Is account banned ? */
    ACCINFO_SLOT_LAST_IP_BAN,       /**< Is last logged (in game) ip banned ? */
    ACCINFO_SLOT_CURR_IP_BAN,       /**< Is current (panel session) ip banned ? */

    ACCINFO_SLOT_COUNT
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
    /// should be true only on page creation
    bool needInfoCreation;

    /// contains account info slots so we can update them in easy way ;)
    PageSlotItem accInfoSlots[ACCINFO_SLOT_COUNT];

    void UpdateTextWidgets();
    void CreateAccountInfo();
    void UpdateAccountInfo();
    void ClearAccountInfo();

    WString GetEmail();
};

#endif // ACCINFO_H_INCLUDED

/********************************************//**
 * \}
 * \}
 ***********************************************/
