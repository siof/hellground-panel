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
#include <Wt/WApplication>
#include <Wt/WBreak>
#include <Wt/WEnvironment>
#include <Wt/WImage>
#include <Wt/WTable>
#include <Wt/WText>

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
    needCreation = true;
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
    else if (needCreation)
        CreateVotePage();

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

    needCreation = false;

    votePageInfo = new WText("");

    addWidget(new WText(tr(TXT_INFO_SUPPORT_VOTE)));
    addWidget(new WBreak());
    addWidget(new WBreak());
    addWidget(votePageInfo);
    addWidget(new WBreak());
    addWidget(new WBreak());

    votes = new Wt::WTable();
    votes->setHeaderCount(1);

    votes->elementAt(0, 0)->addWidget(new WText(""));
    votes->elementAt(0, 1)->addWidget(new WText(tr(TXT_GEN_NAME)));
    votes->elementAt(0, 2)->addWidget(new WText(tr(TXT_GEN_STATE)));

    addWidget(votes);

    if (!wApp->environment().javaScript() || !wApp->environment().ajax())
    {
        votePageInfo->setText(tr(TXT_ERROR_NEED_JAVA_SCRIPT));
        return;
    }

    Database db;

    if (!db.Connect(PANEL_DB_DATA, SQL_PANELDB))
    {
        votePageInfo->setText(tr(TXT_ERROR_DB_CANT_CONNECT));
        return;
    }

    // delete expired votes
    db.ExecutePQuery("DELETE FROM AccVote WHERE reset_date < NOW()", session->accid);
    db.ExecutePQuery("DELETE FROM IPVote WHERE reset_date < NOW()", session->sessionIp.toUTF8().c_str());

    // load vote lists
    switch (db.ExecuteQuery("SELECT id, url, img_url, alt_text, name FROM Vote"))
    {
        case DB_RESULT_ERROR:
            votePageInfo->setText(tr(TXT_ERROR_DB_QUERY_ERROR));
            break;
        case DB_RESULT_EMPTY:
        default:
        {
            std::vector<DatabaseRow*> rows = db.GetRows();
            DatabaseRow * tmpRow;

            for (std::vector<DatabaseRow*>::const_iterator itr = rows.begin(); itr != rows.end(); ++itr)
            {
                tmpRow = *itr;

                VoteInfo tmpInfo;
                tmpInfo.voteId = tmpRow->fields[0].GetUInt32();
                tmpInfo.url = tmpRow->fields[1].GetWString();
                tmpInfo.imgUrl = tmpRow->fields[2].GetString();
                tmpInfo.altText = tmpRow->fields[3].GetWString();
                tmpInfo.voteName = tmpRow->fields[4].GetWString();

                votesInfo.push_back(tmpInfo);
            }

            break;
        }
    }

    // load cooldowns
    db.SetPQuery("SELECT vote_id, reset_date FROM AccVote WHERE account_id = '%u' "
                "UNION "
                "SELECT vote_id, reset_date FROM IPVote WHERE ip = '%s'", session->accid, session->sessionIp.toUTF8().c_str());

    switch (db.ExecuteQuery())
    {
        case DB_RESULT_ERROR:
            votePageInfo->setText(tr(TXT_ERROR_DB_QUERY_ERROR));
            break;
        case DB_RESULT_EMPTY:
        default:
        {
            std::vector<DatabaseRow*> rows = db.GetRows();
            DatabaseRow * tmpRow;
            for (std::vector<DatabaseRow*>::const_iterator itr = rows.begin(); itr != rows.end(); ++itr)
            {
                tmpRow = *itr;

                for (std::vector<VoteInfo>::iterator vItr = votesInfo.begin(); vItr != votesInfo.end(); ++vItr)
                {
                    if ((*vItr).voteId == tmpRow->fields[0].GetUInt32())
                    {
                        (*vItr).disabled = true;

                        if ((*vItr).expire.empty() || (*vItr).expire < tmpRow->fields[1].GetWString())
                            (*vItr).expire = tmpRow->fields[1].GetWString();

                        break;
                    }
                }
            }

            break;
        }
    }

    int i = 1;
    for (std::vector<VoteInfo>::iterator itr = votesInfo.begin(); itr != votesInfo.end(); ++itr, ++i)
    {
        Wt::WAnchor * tmpAnch = new WAnchor(WLink((*itr).url.toUTF8()));
        tmpAnch->setImage(new WImage(WLink((*itr).imgUrl.toUTF8()), (*itr).altText));
        tmpAnch->setTarget(TargetNewWindow);
        tmpAnch->setDisabled((*itr).disabled);

        BindVote(tmpAnch->clicked(), (*itr).voteId);

        votes->elementAt(i, 0)->addWidget(tmpAnch);
        votes->elementAt(i, 1)->addWidget(new WText((*itr).voteName));
        if ((*itr).disabled)
            votes->elementAt(i, 2)->addWidget(new WText(tr(TXT_SUPPORT_VOTE_NEXT).arg((*itr).expire)));
        else
            votes->elementAt(i, 2)->addWidget(new WText(tr(TXT_SUPPORT_VOTE_READY)));

        (*itr).index = i;
    }
}

/********************************************//**
 * \brief Clears page.
 ***********************************************/

void VotePage::ClearPage()
{
    console(DEBUG_CODE, "void VotePage::ClearPage()\n");
    votesInfo.clear();
    clear();
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
            votePageInfo->setText(tr(TXT_ERROR_CANT_VOTE_TWICE));
            return;
        }

        tmpAnch->setDisabled(true);
    }

    VoteInfo * currVote;

    for (std::vector<VoteInfo>::iterator itr = votesInfo.begin(); itr != votesInfo.end(); ++itr)
    {
        if ((*itr).voteId == id)
        {
            currVote = &(*itr);
            break;
        }
    }

    if (currVote->disabled)
    {
        votePageInfo->setText(tr(TXT_ERROR_CANT_VOTE_TWICE));
        return;
    }

    currVote->disabled = true;

    Database db;

    if (!db.Connect(PANEL_DB_DATA, SQL_PANELDB))
    {
        votePageInfo->setText(tr(TXT_ERROR_DB_CANT_CONNECT));
        return;
    }

    if (db.ExecutePQuery("SELECT NOW() + INTERVAL %u HOUR", VOTE_INTERVAL) > DB_RESULT_EMPTY)
        currVote->expire = db.GetRow()->fields[0].GetWString();
    else
    {
        votePageInfo->setText(tr(TXT_ERROR_DB_QUERY_ERROR));
        return;
    }

    db.SetPQuery("INSERT INTO AccVote VALUES ('%u', '%u', '%s')", session->accid, id, currVote->expire.toUTF8().c_str());
    if (db.ExecuteQuery() == DB_RESULT_ERROR)
    {
        votePageInfo->setText(tr(TXT_ERROR_DB_QUERY_ERROR));
        return;
    }

    db.SetPQuery("INSERT INTO IPVote VALUES ('%s', '%u', '%s')", session->sessionIp.toUTF8().c_str(), id, currVote->expire.toUTF8().c_str());
    if (db.ExecuteQuery() == DB_RESULT_ERROR)
    {
        votePageInfo->setText(tr(TXT_ERROR_DB_QUERY_ERROR));
        return;
    }

    if (!db.Connect(SERVER_DB_DATA, SQL_REALMDB))
    {
        votePageInfo->setText(tr(TXT_ERROR_DB_CANT_CONNECT));
        return;
    }

    db.SetPQuery("UPDATE account SET vote = '%u' WHERE id = '%u'", ++session->vote, session->accid);
    if (db.ExecuteQuery() == DB_RESULT_ERROR)
    {
        session->vote--;
        votePageInfo->setText(tr(TXT_ERROR_DB_QUERY_ERROR));
        return;
    }

    votePageInfo->setText(tr(TXT_SUPPORT_VOTED));

    ((WText*)votes->elementAt(currVote->index, 2)->widget(0))->setText(tr(TXT_SUPPORT_VOTE_NEXT).arg(currVote->expire));
}
