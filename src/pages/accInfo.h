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
 * \addtogroup Account Informations
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

/********************************************//**
 * \brief Slots for Account Informations page
 *
 * Also determines order on page.
 *
 ***********************************************/

enum AccountInfoSlot
{
    ACCINFO_SLOT_INFO   = 0,        /**< Page title text */
    ACCINFO_SLOT_TYPE,              /**< Text with account expansion name */
    ACCINFO_SLOT_CURRENT_IP,        /**< Current (panel session) ip */
    ACCINFO_SLOT_CREATE_DATE,       /**< Account create time */
    ACCINFO_SLOT_LAST_LOGIN_DATE,   /**< Last login date */
    ACCINFO_SLOT_LAST_LOGGED_IP,    /**< Last logged (in game) ip */
    ACCINFO_SLOT_IP_LOCK,           /**< Is ip lock on or not ? */
    ACCINFO_SLOT_ONLINE,            /**< Account is actually online */
    ACCINFO_SLOT_CLIENT_VERSION,    /**< Players client locale name */
    ACCINFO_SLOT_VOTE_POINTS,       /**< Vote points count */
    ACCINFO_SLOT_MULTIACC,          /**< Multiacc feature is enabled ? */
    ACCINFO_SLOT_ACC_BAN,           /**< Is account banned ? */
    ACCINFO_SLOT_LAST_IP_BAN,       /**< Is last logged (in game) ip banned ? */
    ACCINFO_SLOT_CURR_IP_BAN,       /**< Is current (panel session) ip banned ? */

    ACCINFO_SLOT_COUNT
};

/********************************************//**
 * \brief Contains text label and single information widget.
 *
 * Each slot item can contain text label and widget which should be displayed on page.
 * Class can also update text labels depends on actual language but
 * this needs support from db and session informations.
 * Break count tells how much new lines should be added after information widget.
 * Text label and information widget are in the same line (no new lines beetwean them).
 *
 ***********************************************/

class AccountInfoSlotItem
{
public:
    AccountInfoSlotItem() : textId(0), breaks(0), label(NULL), widget(NULL) {}
    AccountInfoSlotItem(WText * lbl, WWidget * wid) : textId(0), breaks(0), label(lbl), widget(wid) {}
    ~AccountInfoSlotItem();

    void UpdateLabel(SessionInfo * sess);

    void SetLabel(WText * lbl);
    void SetWidget(WWidget * wid);
    void SetBreakCount(int br) { breaks = br; }
    void SetTextId(uint32 txtId) { textId = txtId; }

    void SetAll(WText * txt, WWidget * wid, int br, uint32 txtId);

    WText * GetLabel() { return label; }
    WWidget * GetWidget() { return widget; }
    int GetBreakCount() { return breaks; }
private:
    uint32 textId;
    int breaks;
    WText * label;
    WWidget * widget;
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
    AccountInfoSlotItem accInfoSlots[ACCINFO_SLOT_COUNT];

    void UpdateTextWidgets();
    void CreateAccountInfo();
    void UpdateAccountInfo();
};

#endif // ACCINFO_H_INCLUDED

/********************************************//**
 * \}
 * \}
 ***********************************************/
