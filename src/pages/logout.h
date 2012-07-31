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
 * \addtogroup Logout Logout page
 * \{
 *
 * \file logout.h
 * This file contains headers needed to show and handle logout page.
 *
 ***********************************************/

#ifndef LOGOUT_H_INCLUDED
#define LOGOUT_H_INCLUDED

#include <Wt/WContainerWidget>

#include "../defines.h"

/********************************************//**
 * \brief A class to represent Logout page.
 *
 * This class is container for widgets for
 * logout page. Text depends on language and
 * is stored in DB.
 *
 ***********************************************/

class LogoutPage : public Wt::WContainerWidget
{
public:
    LogoutPage(SessionInfo * session, Wt::WTemplate * tmpl, Wt::WContainerWidget * parent = 0);
    ~LogoutPage() { clear(); }

    void refresh();
private:
    Wt::WPushButton * btn;
    SessionInfo * sess;
    Wt::WTemplate * templ;

    void CreateLogoutPage();

    void Logout();
};

#endif // LOGOUT_H_INCLUDED

/********************************************//**
 * \}
 * \}
 ***********************************************/
