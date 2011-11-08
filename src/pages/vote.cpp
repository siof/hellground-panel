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
    console(DEBUG_CODE, "void VotePage::refresh()\n");

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

            for (std::map<uint32, VoteSlotItem>::iterator itr = voteMap.begin(); itr != voteMap.end(); ++itr)
                itr->second.UpdateExpireLabel(session);
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
    console(DEBUG_CODE, "void VotePage::CreateVotePage()\n");
    infoSlots[VOTE_SLOT_MAIN].SetLabel(session, TXT_SUPPORT_VOTE_INFO);
    infoSlots[VOTE_SLOT_INFO].SetLabel("");

    addWidget(infoSlots[VOTE_SLOT_MAIN].GetLabel());
    addWidget(new WBreak());
    addWidget(new WBreak());
    addWidget(infoSlots[VOTE_SLOT_INFO].GetLabel());
    addWidget(new WBreak());
    addWidget(new WBreak());
    addWidget(new WBreak());

    if (!wApp->environment().javaScript() || !wApp->environment().ajax())
    {
        infoSlots[VOTE_SLOT_INFO].SetLabel(session, TXT_ERROR_NEED_JAVA_SCRIPT);
        return;
    }

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
            uint32 voteId;
            for (std::list<DatabaseRow*>::const_iterator itr = rows.begin(); itr != rows.end(); ++itr)
            {
                tmpRow = *itr;
                voteId = tmpRow->fields[0].GetUInt32();
                voteMap[voteId].SetExpire(tmpRow->fields[1].GetWString());
                voteMap[voteId].SetDisabled(true);
                voteMap[voteId].SetVoteId(voteId);
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
            std::list<DatabaseRow*> rows = db.GetRows();
            DatabaseRow * tmpRow;
            WAnchor * tmpWidget;
            uint32 voteId;

            for (std::list<DatabaseRow*>::const_iterator itr = rows.begin(); itr != rows.end(); ++itr)
            {
                tmpRow = *itr;

                voteId = tmpRow->fields[0].GetUInt32();

                tmpWidget = new WAnchor(WLink(tmpRow->fields[1].GetString()));
                tmpWidget->setImage(new WImage(WLink(tmpRow->fields[2].GetString()), tmpRow->fields[3].GetWString()));
                tmpWidget->setTarget(TargetNewWindow);
                tmpWidget->setDisabled(voteMap[voteId].IsDisabled());

                BindVote(tmpWidget->clicked(), voteId);

                voteMap[voteId].SetName(tmpRow->fields[4].GetWString());
                WString tmpStr = tmpRow->fields[4].GetWString();

                if (voteMap[voteId].IsDisabled())
                    tmpStr += session->GetText(TXT_SUPPORT_VOTE_NEXT) + voteMap[voteId].GetExpire();

                voteMap[voteId].SetAll(new WText(tmpStr), tmpWidget, 2);
            }

            break;
        }
    }

    WText * tmpText;
    WWidget * tmpWidget;
    int breakes;

    for (std::map<uint32, VoteSlotItem>::iterator itr = voteMap.begin(); itr != voteMap.end(); ++itr)
    {
        tmpText = itr->second.GetLabel();
        tmpWidget = itr->second.GetWidget();
        breakes = itr->second.GetBreakCount();

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
    console(DEBUG_CODE, "void VotePage::ClearPage()\n");
    clear();

    voteMap.clear();

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
    console(DEBUG_CODE, "void VotePage::Vote(const uint32& id = %u)\n", id);
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

    if (voteMap[id].IsDisabled())
    {
        infoSlots[VOTE_SLOT_INFO].SetLabel(session, TXT_ERROR_CANT_VOTE_TWICE);
        return;
    }

    voteMap[id].SetDisabled(true);

    Database db;

    if (!db.Connect(PANEL_DB_DATA, SQL_PANELDB))
    {
        infoSlots[VOTE_SLOT_INFO].SetLabel(session, TXT_DBERROR_CANT_CONNECT);
        return;
    }

    if (db.ExecutePQuery("SELECT NOW() + INTERVAL %u HOUR", VOTE_INTERVAL) > RETURN_EMPTY)
    {
        voteMap[id].SetExpire(db.GetRow()->fields[0].GetWString());
    }
    else
    {
        infoSlots[VOTE_SLOT_INFO].SetLabel(session, TXT_DBERROR_QUERY_ERROR);
        return;
    }

    db.SetPQuery("INSERT INTO AccVote VALUES ('%u', '%u', '%s')", session->accid, id, voteMap[id].GetExpire().toUTF8().c_str());
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

    db.SetPQuery("UPDATE account SET vote = '%u' WHERE id = '%u'", ++session->vote, session->accid);
    if (db.ExecuteQuery() == RETURN_ERROR)
    {
        session->vote--;
        infoSlots[VOTE_SLOT_INFO].SetLabel(session, TXT_DBERROR_QUERY_ERROR);
        return;
    }

    infoSlots[VOTE_SLOT_INFO].SetLabel(session, TXT_SUPPORT_VOTED);
    voteMap[id].UpdateExpireLabel(session);
}
