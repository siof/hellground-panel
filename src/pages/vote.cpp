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

#include <Wt/WAnchor>

#include "../database.h"

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
    setContentAlignment(AlignCenter);
    CreateVotePage();
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
    if (session->accLvl <= LVL_NOT_LOGGED)
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

    voteInfo = new WText("");

    addWidget(new WText(tr(TXT_INFO_SUPPORT_VOTE)));
    addWidget(new WBreak());
    addWidget(new WBreak());
    addWidget(voteInfo);
    addWidget(new WBreak());
    addWidget(new WBreak());

    if (!wApp->environment().javaScript() || !wApp->environment().ajax())
    {
        voteInfo->setText(tr(TXT_ERROR_NEED_JAVA_SCRIPT));
        return;
    }

    Database db;

    if (!db.Connect(PANEL_DB_DATA, SQL_PANELDB))
    {
        voteInfo->setText(tr(TXT_ERROR_DB_CANT_CONNECT));
        return;
    }

    db.ExecutePQuery("DELETE FROM AccVote WHERE acc = '%u' AND resetDate < NOW()", session->accid);
    db.ExecutePQuery("DELETE FROM IPVote WHERE ip = '%s' AND resetDate < NOW()", session->sessionIp.toUTF8().c_str());

    db.SetPQuery("SELECT voteId, resetDate FROM AccVote WHERE acc = '%u'", session->accid);

    switch (db.ExecuteQuery())
    {
        case DB_RESULT_ERROR:
            voteInfo->setText(tr(TXT_ERROR_DB_QUERY_ERROR));
            break;
        case DB_RESULT_EMPTY:
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

    db.SetPQuery("SELECT voteId, resetDate FROM IPVote WHERE ip = '%s'", session->sessionIp.toUTF8().c_str());

    switch (db.ExecuteQuery())
    {
        case DB_RESULT_ERROR:
            voteInfo->setText(tr(TXT_ERROR_DB_QUERY_ERROR));
            break;
        case DB_RESULT_EMPTY:
        default:
        {
            std::list<DatabaseRow*> rows = db.GetRows();
            DatabaseRow * tmpRow;
            uint32 voteId;

            for (std::list<DatabaseRow*>::const_iterator itr = rows.begin(); itr != rows.end(); ++itr)
            {
                tmpRow = *itr;
                voteId = tmpRow->fields[0].GetUInt32();
                if (!voteMap[voteId].IsDisabled())
                {
                    voteMap[voteId].SetExpire(tmpRow->fields[1].GetWString());
                    voteMap[voteId].SetDisabled(true);
                }

                voteMap[voteId].SetVoteId(voteId);
            }

            break;
        }
    }

    switch (db.ExecuteQuery("SELECT id, url, imgurl, alttext, name FROM Vote"))
    {
        case DB_RESULT_ERROR:
            voteInfo->setText(tr(TXT_ERROR_DB_QUERY_ERROR));
            break;
        case DB_RESULT_EMPTY:
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
                WString tmpStr;

                if (voteMap[voteId].IsDisabled())
                    tmpStr = tr(TXT_SUPPORT_VOTE_NEXT).arg(tmpRow->fields[4].GetWString()).arg(voteMap[voteId].GetExpire());
                else
                    tmpStr = tmpRow->fields[4].GetWString();

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
    voteMap.clear();
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
            voteInfo->setText(tr(TXT_ERROR_CANT_VOTE_TWICE));
            return;
        }

        tmpAnch->setDisabled(true);
    }

    if (voteMap[id].IsDisabled())
    {
        voteInfo->setText(tr(TXT_ERROR_CANT_VOTE_TWICE));
        return;
    }

    voteMap[id].SetDisabled(true);

    Database db;

    if (!db.Connect(PANEL_DB_DATA, SQL_PANELDB))
    {
        voteInfo->setText(tr(TXT_ERROR_DB_CANT_CONNECT));
        return;
    }

    if (db.ExecutePQuery("SELECT NOW() + INTERVAL %u HOUR", VOTE_INTERVAL) > DB_RESULT_EMPTY)
    {
        voteMap[id].SetExpire(db.GetRow()->fields[0].GetWString());
    }
    else
    {
        voteInfo->setText(tr(TXT_ERROR_DB_QUERY_ERROR));
        return;
    }

    db.SetPQuery("INSERT INTO AccVote VALUES ('%u', '%u', '%s')", session->accid, id, voteMap[id].GetExpire().toUTF8().c_str());
    if (db.ExecuteQuery() == DB_RESULT_ERROR)
    {
        voteInfo->setText(tr(TXT_ERROR_DB_QUERY_ERROR));
        return;
    }

    db.SetPQuery("INSERT INTO IPVote VALUES ('%s', '%u', '%s')", session->sessionIp.toUTF8().c_str(), id, voteMap[id].GetExpire().toUTF8().c_str());
    if (db.ExecuteQuery() == DB_RESULT_ERROR)
    {
        voteInfo->setText(tr(TXT_ERROR_DB_QUERY_ERROR));
        return;
    }

    if (!db.Connect(SERVER_DB_DATA, SQL_REALMDB))
    {
        voteInfo->setText(tr(TXT_ERROR_DB_CANT_CONNECT));
        return;
    }

    db.SetPQuery("UPDATE account SET vote = '%u' WHERE id = '%u'", ++session->vote, session->accid);
    if (db.ExecuteQuery() == DB_RESULT_ERROR)
    {
        session->vote--;
        voteInfo->setText(tr(TXT_ERROR_DB_QUERY_ERROR));
        return;
    }

    voteInfo->setText(tr(TXT_SUPPORT_VOTED));
    voteMap[id].UpdateExpireLabel();
}
