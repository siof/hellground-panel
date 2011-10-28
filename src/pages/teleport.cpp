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
 * \{
 *
 * \file teleport.cpp
 * This file contains code needed to handle character teleport/unstack.
 *
 ***********************************************/

#include "teleport.h"
#include "../database.h"

TeleportPage::TeleportPage(SessionInfo * sess, WContainerWidget * parent):
    WContainerWidget(parent), session(sess), guids(NULL)
{
    console(DEBUG_CODE, "TeleportPage::TeleportPage(SessionInfo * sess = %i, WContainerWidget * parent = %i)", sess != NULL, parent != NULL);
    setContentAlignment(AlignCenter|AlignTop);

    teleportSlots[TELEPORT_SLOT_INFO].SetText(sess, TXT_TELEPORT);
    teleportSlots[TELEPORT_SLOT_STATUS].SetText("");

    addWidget(teleportSlots[TELEPORT_SLOT_INFO].GetText());
    addWidget(new WBreak());
    addWidget(teleportSlots[TELEPORT_SLOT_STATUS].GetText());
    addWidget(new WBreak());
    addWidget(new WBreak());
    addWidget(new WBreak());

    characters = new WComboBox();
    addWidget(characters);
    addWidget(new WBreak());
    addWidget(new WBreak());

    btnTeleport = new WPushButton(sess->GetText(TXT_BTN_TELEPORT));
    btnTeleport->clicked().connect(this, &TeleportPage::Teleport);
    addWidget(btnTeleport);
}

TeleportPage::~TeleportPage()
{
    delete [] guids;
}

/********************************************//**
 * \brief Overloads WContainerWidget::refresh() for automatic content change.
 *
 * This function can create new or update existing content.
 * In most situations this is used for content update ;)
 *
 ***********************************************/

void TeleportPage::refresh()
{
    #ifdef DEBUG
    printf("TeleportPage::refresh()");
    #endif

    // teleport page should be only for not logged yet players so there is no need to update it in other cases
    if (session->accLvl > LVL_NOT_LOGGED)
    {
        UpdateTextWidgets();
        LoadCharacters();
    }

    WContainerWidget::refresh();
}

/********************************************//**
 * \brief Update text widgets.
 *
 * All text label widgets in all slots will be updated,
 * so if player will change language then automagically
 * labels should change too ;)
 *
 ***********************************************/

void TeleportPage::UpdateTextWidgets()
{
    for (int i = 0; i < TELEPORT_SLOT_COUNT; ++i)
        teleportSlots[i].UpdateText(session);

    btnTeleport->setText(session->GetText(TXT_BTN_TELEPORT));
}

/********************************************//**
 * \brief Load characters from DB.
 *
 * This function gets characters from DB and
 * puts them into guid table and combobox from which
 * user can choose character to teleport/unstack.
 *
 ***********************************************/

void TeleportPage::LoadCharacters()
{
    delete [] guids;

    characters->clear();

    Database * db = new Database();
    if (db->Connect(SERVER_DB_DATA, SQL_CHARDB))
    {
        db->SetPQuery("SELECT guid, name FROM characters WHERE account = %u", session->accid);

        if (db->ExecuteQuery() > RETURN_EMPTY)
        {
            std::list<DatabaseRow*> rows = db->GetRows();
            DatabaseRow * tmpRow;

            guids = new uint64[rows.size()];
            int i = 0;

            for (std::list<DatabaseRow*>::const_iterator itr = rows.begin(); itr != rows.end(); ++itr, ++i)
            {
                tmpRow = *itr;

                guids[i] = tmpRow->fields[0].GetUInt64();

                characters->addItem(tmpRow->fields[1].GetWString());
            }
        }
    }

    delete db;
}

/********************************************//**
 * \brief Teleports character.
 *
 * Function to 'physically' teleport choosen character.
 * Here also should be done check if character if offline
 * (we can teleport only offline characters).
 * In addition place where character will be teleported
 * should depend on character race (teleport should be done
 * to race start location).
 *
 ***********************************************/

void TeleportPage::Teleport()
{
    if (session->accLvl < LVL_PLAYER)
        return;

    if (characters->count() < 1)
        return;

    int index = characters->currentIndex();

    if (index < 0 || index >= characters->count())
        return;

    uint32 teleportStatus;

    Database db;
    bool success = false;
    WString name = characters->currentText();

    if (db.Connect(SERVER_DB_DATA, SQL_CHARDB))
    {
        db.SetPQuery("SELECT online, race, name FROM characters WHERE guid = %u", guids[index]);

        switch (db.ExecuteQuery())
        {
            case RETURN_ERROR:
                teleportStatus = TXT_DBERROR_QUERY_ERROR;
                break;
            case RETURN_EMPTY:
                teleportStatus = TXT_ERROR_CHARACTER_NOT_FOUND;
                break;
            default:
                DatabaseRow * tmpRow = db.GetRow();

                if (tmpRow->fields[0].GetBool() == false)
                {
                    Location loc;
                    GetTeleportPosition(tmpRow->fields[1].GetInt(), loc);

                    db.SetPQuery("UPDATE characters SET map = '%u', position_x = '%f', position_y = '%f', position_z = '%f' WHERE guid = '%u'", loc.mapId, loc.posX, loc.posY, loc.posZ, guids[index]);

                    if (db.ExecuteQuery() != RETURN_ERROR)
                    {
                        db.ExecutePQuery("REPLACE INTO character_spell_cooldown VALUES (%u, 8690, 0, unix_timestamp()+3600)", guids[index]);
                        teleportStatus = TXT_TELEPORT_SUCCESSFULL;
                        success = true;
                    }
                    else
                        teleportStatus = TXT_DBERROR_QUERY_ERROR;
                }
                else
                    teleportStatus = TXT_ERROR_CANT_TELEPORT_ONLINE;
                break;
        }
    }
    else
        teleportStatus = TXT_DBERROR_CANT_CONNECT;

    teleportSlots[TELEPORT_SLOT_STATUS].SetText(session, teleportStatus);

    if (db.Connect(PANEL_DB_DATA, SQL_PANELDB))
    {
        std::string tmpStr = GetFormattedString("Teleport. Character: %s. success: %s", db.EscapeString(name).c_str(), success ? "Yes" : "No");
        db.ExecutePQuery("INSERT INTO Activity VALUES ('XXX', '%u', NOW(), '%s', 0, '%s')", session->accid, session->sessionIp.toUTF8().c_str(), tmpStr.c_str());
    }
}

/********************************************//**
 * \}
 * \}
 *
 ***********************************************/
