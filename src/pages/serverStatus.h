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
 * \addtogroup ServerStatusPage Server status page
 * \{
 *
 * \file serverStatus.h
 * This file contains headers needed to show page with server status.
 *
 ***********************************************/

#ifndef SERVERSTATUS_H_INCLUDED
#define SERVERSTATUS_H_INCLUDED

#include <Wt/WContainerWidget>

#include "../defines.h"

/********************************************//**
 * \brief Slots for server status page
 *
 * Also determines order on page.
 *
 ***********************************************/

enum ServerStatusTextSlots
{
    SERVER_STATUS_TEXT_REALM = 0,       /**< Realm name. */
    SERVER_STATUS_TEXT_STATE,           /**< Realm state (online/offline). */
    SERVER_STATUS_TEXT_ONLINE,          /**< Current players online count. */
    SERVER_STATUS_TEXT_MAXONLINE,       /**< Max online players. */
    SERVER_STATUS_TEXT_FACTIONS,        /**< Show players online in factions (in percents). */
    SERVER_STATUS_TEXT_UPTIME,          /**< Current realm uptime. */
    SERVER_STATUS_TEXT_REVISION,        /**< Actual realm rev. */
    SERVER_STATUS_TEXT_DIFF,            /**< Avarage diff from last minute. */
    SERVER_STATUS_TEXT_AVGDIFF,         /**< Avarage diff from last dc/crash/off. */
    SERVER_STATUS_TEXT_INFO,            /**< Additional info to show. */

    SERVER_STATUS_TEXT_SLOT_COUNT
};

/********************************************//**
 * \brief A class to represent Server status page.
 *
 * This class is container for widgets contains
 * text and data for server status page.
 * Text depends on language and is stored in DB.
 * Data are parsed from server output file.
 *
 ***********************************************/

class ServerStatusPage : public WContainerWidget
{
public:
    ServerStatusPage(WContainerWidget * parent = 0);
    ~ServerStatusPage() { clear(); }

    void refresh();
private:
    /// status refresh timer
    Wt::WTimer * timer;
    /// tables for multiple realm status
    Wt::WTable * realms[REALMS_COUNT];

    void CreateStatusPage();
    void UpdateStatus();
};

#endif // SERVERSTATUS_H_INCLUDED

/********************************************//**
 * \}
 * \}
 ***********************************************/
