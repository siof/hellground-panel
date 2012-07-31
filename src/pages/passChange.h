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
 * \addtogroup Passwords Management
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

#include <Wt/WContainerWidget>

#include "../defines.h"

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

    /// label with informations for changing password
    WText * changeInfo;

    /// text box for passwords
    WLineEdit * txtPassOld;
    WLineEdit * txtPass;
    WLineEdit * txtPass2;

    /// register button
    WPushButton * btnChange;

    void CreatePassChangePage();

    void ClearPass();
    void Change();
};

#endif // PASSCHANGE_H_INCLUDED

/********************************************//**
 * \}
 * \}
 ***********************************************/
