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
 * \addtogroup Password recovery
 * Password recovery page contains code to handle
 * player password recovery.
 * Password recovery consists of mostly 2 files:
 * - passRecovery.h
 * - passRecovery.cpp
 * \{
 *
 * \file passRecovery.h
 * This file contains headers needed to player password recovery.
 *
 ***********************************************/

#ifndef PASSRECOVERY_H_INCLUDED
#define PASSRECOVERY_H_INCLUDED

#include "../defines.h"
#include "../textItems.h"

/********************************************//**
 * \brief Slots for password recovery page
 *
 * Also determines order on page.
 *
 ***********************************************/

enum RecoveryTextSlots
{
    RECOVERY_TEXT_MAIN = 0,         /**< Page title/main text shown on top. */
    RECOVERY_TEXT_INFO,             /**< Additional info/recovery errors. */
    RECOVERY_TEXT_LOGIN,            /**< New account login. */
    RECOVERY_TEXT_EMAIL,            /**< New account mail. On this mail password will be sent. */

    RECOVERY_TEXT_SLOT_COUNT
};


/********************************************//**
 * \brief A class to represents Password Recovery page
 *
 * This class is container for other widgets needed to recovery account password.
 * Content should be created and added only on page creation.
 * All informations in widgets and texts which depend on language
 * should be only updated after page creation.
 * This page should also validate data and handle
 * recovery steps (password generation, change in db and send mail with pass).
 *
 ***********************************************/

class PassRecoveryPage : public WContainerWidget
{
public:
    PassRecoveryPage(SessionInfo * sess, WContainerWidget * parent = 0);
    ~PassRecoveryPage();

    void refresh();
private:
    /// panel session informations
    SessionInfo * session;
    /// should be true only on page creation
    bool needCreation;

    /// text box for login
    WLineEdit * txtLogin;
    /// text box for email
    WLineEdit * txtEmail;
    /// register button
    WPushButton * btnRecover;

    /// labels
    TextItems textSlots[RECOVERY_TEXT_SLOT_COUNT];

    void UpdateTextWidgets();
    void CreateRecoveryPage();

    void ClearLogin();
    void ClearEmail();
    void Recover();
};


#endif // PASSRECOVERY_H_INCLUDED

/********************************************//**
 * \}
 * \}
 ***********************************************/
