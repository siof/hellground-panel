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
#include <Wt/WCheckBox>

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

    /// label for registration informations
    WText * regInfo;

    /// text box for login
    WLineEdit * txtLogin;
    /// text box for email
    WLineEdit * txtEmail;
    /// register button
    WPushButton * btnRegister;
    /// check box server rules accepting
    WCheckBox * chRules;

    void CreateRegisterPage();

    void ClearRegisterData();
    void CheckChange();
    void Register();

    void ClearWLineEdit()
    {
        if (WObject::sender())
            ((WLineEdit*)WObject::sender())->setText("");
    }
};


#endif // REGISTER_H_INCLUDED

/********************************************//**
 * \}
 * \}
 ***********************************************/
