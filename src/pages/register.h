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
 * \addtogroup Pages
 * \{
 *
 * \addtogroup Accounts Account Management
 * Registration page contains code to handle player registration.
 * Registration consists of mostly 2 files:
 * - register.h
 * - register.cpp
 * \{
 *
 * \file register.h
 * This file contains headers needed to handle player registration.
 *
 ***********************************************/

#ifndef REGISTER_H_INCLUDED
#define REGISTER_H_INCLUDED

#include "../defines.h"
#include "../slotItems.h"
#include <WCheckBox>

/********************************************//**
 * \brief Slots for Registration page
 *
 ***********************************************/

enum RegistrationTextSlots
{
    REG_TEXT_MAIN = 0,          /**< Page title/main text shown on top. */
    REG_TEXT_INFO,              /**< Additional info/registration errors like trying to register with existing username. */
    REG_TEXT_LOGIN,             /**< New account login. */
    REG_TEXT_EMAIL,             /**< New account mail. On this mail password will be sent. */
    REG_TEXT_RULES,             /**< Server rules information. */
//    REG_TEXT_RULES_ACCEPT,

    REG_TEXT_SLOT_COUNT
};

/********************************************//**
 * \brief A class to represents Registration page
 *
 * This class is container for other widgets needed to account registration.
 * Content should be created and added only on page creation.
 * All informations in widgets and texts which depend on language
 * should be only updated after page creation.
 * This page should also validate registration data and handle
 * registration steps (password generation, add to db and send mail with pass).
 *
 ***********************************************/

class RegisterPage : public WContainerWidget
{
public:
    RegisterPage(SessionInfo * sess, WContainerWidget * parent = 0);
    ~RegisterPage();

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
    WPushButton * btnRegister;
    /// check box server rules accepting
    WCheckBox * chRules;

    /// labels
    BasicTextItem textSlots[REG_TEXT_SLOT_COUNT];

    void UpdateTextWidgets();
    void CreateRegisterPage();

    void ClearLogin();
    void ClearEmail();
    void CheckChange();
    void Register();
};


#endif // REGISTER_H_INCLUDED

/********************************************//**
 * \}
 * \}
 ***********************************************/
