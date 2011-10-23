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
 * \addtogroup Licence Licence and informations page
 * \{
 *
 * \file licence.h
 * This file contains headers needed to show informations/licence page.
 *
 ***********************************************/

#ifndef LICENCE_H_INCLUDED
#define LICENCE_H_INCLUDED

#include "../defines.h"
#include "../slotItems.h"

enum LicenceTextSlot
{
    LICENCE_SLOT_INTRO  = 0,
    LICENCE_SLOT_REPO   = 1,
    LICENCE_SLOT_INFO   = 2,

    LICENCE_SLOT_COUNT
};

/********************************************//**
 * \brief A class to represent licence page.
 *
 * This class is container for widgets contains
 * text for home page. Text depends on language and
 * is stored in DB.
 *
 ***********************************************/

class LicencePage : public WContainerWidget
{
public:
    LicencePage(SessionInfo * sess, WContainerWidget * parent = 0);
    ~LicencePage() { clear(); }

    void refresh();
private:
    /// pointer to object with current session informations
    SessionInfo * session;

    bool needCreation;

    BasicTextItem licenceTextSlots[LICENCE_SLOT_COUNT];

    void ShowText();
    void UpdateText();
};

#endif // LICENCE_H_INCLUDED

/********************************************//**
 * \}
 * \}
 ***********************************************/

