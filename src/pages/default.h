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
 * \addtogroup Default Default page
 * \{
 *
 * \file default.h
 * This file contains headers needed to show default (home/start) page.
 *
 ***********************************************/

#ifndef DEFAULT_H_INCLUDED
#define DEFAULT_H_INCLUDED

#include "../defines.h"

/********************************************//**
 * \brief A class to represent Default (home/start) page.
 *
 * This class is container for widgets contains
 * text for home page. Text depends on language and
 * is stored in DB.
 *
 ***********************************************/

class DefaultPage : public WContainerWidget
{
public:
    DefaultPage(SessionInfo * sess, WContainerWidget * parent = 0);
    ~DefaultPage() { clear(); }

    void refresh();
private:
    /// pointer to object with current session informations
    SessionInfo * session;

    void ShowText();
};

#endif // DEFAULT_H_INCLUDED

/********************************************//**
 * \}
 * \}
 ***********************************************/
