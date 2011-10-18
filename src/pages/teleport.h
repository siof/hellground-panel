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
 * \addtogroup Teleport Teleport Page
 * Teleport page contains code to handle character unstack teleport.
 * Teleport consists of mostly 2 files:
 * - teleport.h
 * - teleport.cpp
 * \{
 *
 * \file teleport.h
 * This file contains headers needed to handle character teleport.
 *
 ***********************************************/

#ifndef TELEPORT_H_INCLUDED
#define TELEPORT_H_INCLUDED

#include "../defines.h"
#include "../slotItems.h"

#include <WComboBox>

/********************************************//**
 * \brief Slots for basic text labels.
 *
 * In this slots will be basic texts which depends on language.
 *
 ***********************************************/


enum TeleportPageSlots
{
    TELEPORT_SLOT_INFO      = 0,
    TELEPORT_SLOT_STATUS    = 1,

    TELEPORT_SLOT_COUNT
};

/********************************************//**
 * \brief A class to represent Teleport page
 *
 * This class is container for other widgets needed to Teleport
 * character. Content should be created and added only on page creation.
 * All informations in widgets and texts which depend on language
 * should be only updated after page creation.
 *
 ***********************************************/

class TeleportPage : public WContainerWidget
{
public:
    TeleportPage(SessionInfo * sess, WContainerWidget * parent = 0);
    ~TeleportPage();

    void refresh();

private:
    /// panel session informations
    SessionInfo * session;
    /// slots for static labels
    BasicTextItem teleportSlots[TELEPORT_SLOT_COUNT];
    /// Combo box with character names
    WComboBox * characters;
    /// Button to confirm character teleportation
    WPushButton * btnTeleport;
    /// Pointer to table with character guids (teleport will be done on character with guid from this table)
    uint64 * guids;

    void UpdateTextWidgets();
    void LoadCharacters();
    void Teleport();
};


#endif // TELEPORT_H_INCLUDED

/********************************************//**
 * \}
 * \}
 ***********************************************/
