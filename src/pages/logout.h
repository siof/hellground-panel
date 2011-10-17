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
 * \addtogroup Logout Logout page
 * \{
 *
 * \file logout.h
 * This file contains headers needed to show and handle logout page.
 *
 ***********************************************/

#ifndef LOGOUT_H_INCLUDED
#define LOGOUT_H_INCLUDED

#include "../defines.h"

/********************************************//**
 * \brief A class to represent Logout page.
 *
 * This class is container for widgets for
 * logout page. Text depends on language and
 * is stored in DB.
 *
 ***********************************************/

class LogoutPage : public WContainerWidget
{
public:
    LogoutPage(SessionInfo * sess, WContainerWidget * parent = 0);
    ~LogoutPage() { clear(); }

    void refresh();
private:
    /// pointer to object with current session informations
    SessionInfo * session;
    /// should be true only once per player
    bool needCreation;

    WText * info;
    WPushButton * btn;

    void CreateLogoutPage();
    void UpdateLogoutPage();

    void Logout();
};

#endif // LOGOUT_H_INCLUDED

/********************************************//**
 * \}
 * \}
 ***********************************************/
