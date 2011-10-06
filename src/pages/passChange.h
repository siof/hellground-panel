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
 * \addtogroup Password change
 * Password change page contains code to handle
 * player password change.
 * Password change consists of mostly 2 files:
 * - passChange.h
 * - passChange.cpp
 * \{
 *
 * \file passChange.h
 * This file contains headers needed to player password change.
 *
 ***********************************************/

#ifndef PASSCHANGE_H_INCLUDED
#define PASSCHANGE_H_INCLUDED

#include "../defines.h"
#include "../slotItems.h"

/********************************************//**
 * \brief Slots for password change page
 *
 * Also determines order on page.
 *
 ***********************************************/

enum PassChangeTextSlots
{
    PASS_CHANGE_TEXT_MAIN = 0,      /**< Page title/main text shown on top. */
    PASS_CHANGE_TEXT_INFO,          /**< Additional info/change errors. */
    PASS_CHANGE_TEXT_LOGIN,         /**< Account login. */
    PASS_CHANGE_TEXT_PASS,          /**< New password. */
    PASS_CHANGE_TEXT_PASS2,         /**< New password 2. */

    PASS_CHANGE_TEXT_SLOT_COUNT
};


/********************************************//**
 * \brief A class to represents Password change page
 *
 * This class is container for other widgets needed to change account password.
 * Content should be created and added only on page creation.
 * All informations in widgets and texts which depend on language
 * should be only updated after page creation.
 * This page should also validate data and handle change steps (change in db).
 *
 ***********************************************/

class PassChangePage : public WContainerWidget
{
public:
    PassChangePage(SessionInfo * sess, WContainerWidget * parent = 0);
    ~PassChangePage();

    void refresh();
private:
    /// panel session informations
    SessionInfo * session;
    /// should be true only on page creation
    bool needCreation;

    /// text box for passwords
    WLineEdit * txtPass;
    WLineEdit * txtPass2;
    /// register button
    WPushButton * btnChange;

    /// labels
    BasicTextItem textSlots[PASS_CHANGE_TEXT_SLOT_COUNT];

    void UpdateTextWidgets();
    void CreatePassChangePage();

    void ClearPass();
    void ClearPass2();
    void Change();
};


#endif // PASSCHANGE_H_INCLUDED

/********************************************//**
 * \}
 * \}
 ***********************************************/
