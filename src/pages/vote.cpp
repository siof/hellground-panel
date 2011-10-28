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
 * \addtogroup Support Support project
 * \{
 *
 * \file vote.cpp
 * This file contains code needed to provide vote possibility.
 *
 ***********************************************/

#include "vote.h"
#include "../database.h"
#include <WAnchor>

/********************************************//**
 * \brief Creates new VotePage object.
 *
 * \param sess      Contains user session informations.
 * \param parent    Parent container widget to which should be added this widget.
 *
 ***********************************************/

VotePage::VotePage(SessionInfo * sess, WContainerWidget * parent):
    WContainerWidget(parent), session(sess)
{
    needCreation = true;
    setContentAlignment(AlignCenter);
}

VotePage::~VotePage()
{
    session = NULL;
}

/********************************************//**
 * \brief Overloads WContainerWidget::refresh() for automatic content change.
 *
 * This function can create new or update existing content.
 * In most situations this is used for content update ;)
 *
 ***********************************************/

void VotePage::refresh()
{
    if (isHidden() || isDisabled())
        return;

    // only logged in players can visit this page so there is no need to create/update it in other cases
    if (session->accLvl > LVL_NOT_LOGGED)
    {
        if (needCreation)
        {
            needCreation = false;
            CreateVotePage();
        }
        else
        {
            infoSlots[VOTE_SLOT_MAIN].UpdateLabel(session);
            infoSlots[VOTE_SLOT_INFO].UpdateLabel(session);
        }
    }
    else
        ClearPage();

    WContainerWidget::refresh();
}

/********************************************//**
 * \brief Creates vote page.
 *
 * Function to create all vote buttons based on data
 * from database.
 *
 ***********************************************/

void VotePage::CreateVotePage()
{
    infoSlots[VOTE_SLOT_MAIN].SetLabel(session, TXT_SUPPORT_VOTE_INFO);
    infoSlots[VOTE_SLOT_INFO].SetLabel("");

    addWidget(infoSlots[VOTE_SLOT_MAIN].GetLabel());
    addWidget(new WBreak());
    addWidget(new WBreak());
    addWidget(infoSlots[VOTE_SLOT_INFO].GetLabel());
    addWidget(new WBreak());
    addWidget(new WBreak());
    addWidget(new WBreak());

    Database db;

    if (!db.Connect(PANEL_DB_DATA, SQL_PANELDB))
    {
        infoSlots[VOTE_SLOT_INFO].SetLabel(session, TXT_DBERROR_CANT_CONNECT);
        return;
    }

    db.SetPQuery("SELECT voteId, resetDate FROM AccVote WHERE acc = '%u'", session->accid);

    switch (db.ExecuteQuery())
    {
        case RETURN_ERROR:
            infoSlots[VOTE_SLOT_INFO].SetLabel(session, TXT_DBERROR_QUERY_ERROR);
            break;
        case RETURN_EMPTY:
        default:
        {
            std::list<DatabaseRow*> rows = db.GetRows();
            DatabaseRow * tmpRow;

            for (std::list<DatabaseRow*>::const_iterator itr = rows.begin(); itr != rows.end(); ++itr)
            {
                tmpRow = *itr;

                voteMap[tmpRow->fields[0].GetUInt32()] = tmpRow->fields[1].GetWString();
            }

            break;
        }
    }

    switch (db.ExecuteQuery("SELECT id, url, imgurl, alttext, name FROM Vote"))
    {
        case RETURN_ERROR:
            infoSlots[VOTE_SLOT_INFO].SetLabel(session, TXT_DBERROR_QUERY_ERROR);
            break;
        case RETURN_EMPTY:
        default:
        {
            std::map<uint32, WString>::iterator mapItr;
            std::list<DatabaseRow*> rows = db.GetRows();
            DatabaseRow * tmpRow;
            PageSlotItem * tmpItem;
            WAnchor * tmpWidget;
            bool disabled;

            for (std::list<DatabaseRow*>::const_iterator itr = rows.begin(); itr != rows.end(); ++itr)
            {
                tmpRow = *itr;
                mapItr = voteMap.find(tmpRow->fields[0].GetUInt32());

                disabled = mapItr != voteMap.end();

                tmpWidget = new WAnchor(WLink(tmpRow->fields[1].GetString()));
                tmpWidget->setImage(new WImage(WLink(tmpRow->fields[2].GetString()), tmpRow->fields[3].GetWString()));
                tmpWidget->setTarget(TargetNewWindow);
                tmpWidget->setDisabled(disabled);

                BindVote(tmpWidget->clicked(), tmpRow->fields[0].GetUInt32());

                WString tmpStr = tmpRow->fields[4].GetWString();

                if (disabled)
                    tmpStr += session->GetText(TXT_SUPPORT_VOTE_NEXT) + mapItr->second;

                tmpItem = new PageSlotItem();
                tmpItem->SetAll(new WText(tmpStr), tmpWidget, 2);

                votes.push_back(tmpItem);
            }

            break;
        }
    }

    WText * tmpText;
    WWidget * tmpWidget;
    int breakes;

    for (std::list<PageSlotItem*>::const_iterator itr = votes.begin(); itr != votes.end(); ++itr)
    {
        tmpText = (*itr)->GetLabel();
        tmpWidget = (*itr)->GetWidget();
        breakes = (*itr)->GetBreakCount();

        addWidget(tmpText);
        addWidget(new WBreak());
        addWidget(tmpWidget);

        for (int i = 0; i < breakes; ++i)
            addWidget(new WBreak());
    }
}

/********************************************//**
 * \brief Clears page.
 ***********************************************/

void VotePage::ClearPage()
{
    clear();

    for (std::list<PageSlotItem*>::iterator itr = votes.begin(); itr != votes.end(); ++itr)
        delete (*itr);

    votes.clear();

    for (int i = 0; i < VOTE_SLOT_COUNT; ++i)
        infoSlots[i].Clear();

    needCreation = true;
}

void VotePage::BindVote(EventSignal<WMouseEvent>& signal, const uint32& id)
{
    signal.connect(boost::bind(&VotePage::Vote, this, id));
}

void VotePage::Vote(const uint32& id)
{
    if (WObject::sender())
    {
        WAnchor * tmpAnch = ((WAnchor*)WObject::sender());
        if (tmpAnch->isDisabled())
        {
            infoSlots[VOTE_SLOT_INFO].SetLabel(session, TXT_ERROR_CANT_VOTE_TWICE);
            return;
        }

        tmpAnch->setDisabled(true);
    }

    std::map<uint32, WString>::iterator mapItr = voteMap.find(id);
    if (mapItr != voteMap.end())
    {
        infoSlots[VOTE_SLOT_INFO].SetLabel(session, TXT_ERROR_CANT_VOTE_TWICE);
        return;
    }

    voteMap[id] = "";

    Database db;

    if (!db.Connect(PANEL_DB_DATA, SQL_PANELDB))
    {
        infoSlots[VOTE_SLOT_INFO].SetLabel(session, TXT_DBERROR_CANT_CONNECT);
        return;
    }

    db.SetPQuery("INSERT INTO AccVote VALUES ('%u', '%u', NOW() + INTERVAL %u HOUR)", session->accid, id, VOTE_INTERVAL);
    if (db.ExecuteQuery() == RETURN_ERROR)
    {
        infoSlots[VOTE_SLOT_INFO].SetLabel(session, TXT_DBERROR_QUERY_ERROR);
        return;
    }

    if (!db.Connect(SERVER_DB_DATA, SQL_REALMDB))
    {
        infoSlots[VOTE_SLOT_INFO].SetLabel(session, TXT_DBERROR_CANT_CONNECT);
        return;
    }

    db.SetPQuery("UPDATE account SET vote = '%u' WHERE id = '%u'", ++session->vote);
    if (db.ExecuteQuery() == RETURN_ERROR)
    {
        session->vote--;
        infoSlots[VOTE_SLOT_INFO].SetLabel(session, TXT_DBERROR_QUERY_ERROR);
        return;
    }

    infoSlots[VOTE_SLOT_INFO].SetLabel(session, TXT_SUPPORT_VOTED);
}
