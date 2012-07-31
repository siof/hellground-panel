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
 * \addtogroup Support Support project
 * Support pages contains code to provide possibility
 * to support project by players such as vote/donate.
 * Support consists of mostly 4 files:
 * - support.h
 * - support.cpp
 * - vote.h
 * - vote.cpp
 * \{
 *
 * \file support.h
 * This file contains th headers needed to provide support possibility.
 *
 ***********************************************/

#ifndef SUPPORT_H_INCLUDED
#define SUPPORT_H_INCLUDED

#include <Wt/WContainerWidget>

#include "../defines.h"

/********************************************//**
 * \brief A class to represent Support page.
 *
 * This class is container for widgets contains
 * text for support page. Text depends on language and
 * is stored in DB.
 *
 ***********************************************/

class SupportPage : public WContainerWidget
{
public:
    SupportPage(SessionInfo * sess, WContainerWidget * parent = 0);
    ~SupportPage();

    void refresh();
private:
    /// created or not?
    bool needCreation;
    SessionInfo * session;
};

#endif //SUPPORT_H_INCLUDED
