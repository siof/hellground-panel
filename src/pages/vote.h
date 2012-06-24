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
 * \file vote.h
 * This file contains the headers needed to provide vote possibility.
 *
 ***********************************************/

#ifndef VOTE_H_INCLUDED
#define VOTE_H_INCLUDED

#include <Wt/WContainerWidget>

#include "../defines.h"

/********************************************//**
 * \brief Slots for vote page.
 ***********************************************/

enum VoteSlots
{
    VOTE_SLOT_MAIN  = 0,    /**< Main info text/page title */
    VOTE_SLOT_INFO  = 1,    /**< Slot for additional info setted by vote page while using */

    VOTE_SLOT_COUNT
};

/********************************************//**
 * \brief Structure to store specific vote informations.
 ***********************************************/

struct VoteInfo
{
    VoteInfo() : disabled(false), voteId(0), index(0), expire(""), voteName(""), url(""), imgUrl(""), altText("") {}
    ~VoteInfo(){}

    bool disabled;
    uint32 voteId;
    uint32 index;
    WString expire;
    WString voteName;
    WString url;
    WString imgUrl;
    WString altText;
};

/********************************************//**
 * \brief A class to represent Vote page.
 *
 * This class is container for widgets contains
 * text for support page. Text depends on language and
 * is stored in DB.
 *
 ***********************************************/

class VotePage : public WContainerWidget
{
public:
    VotePage(SessionInfo * sess, WContainerWidget * parent = 0);
    ~VotePage();

    void refresh();
private:
    /// panel session informations
    SessionInfo * session;
    /// info if page should be created
    bool needCreation;
    /// label with vote informations
    WText * votePageInfo;
    /// Table with votes
    Wt::WTable * votes;

    void CreateVotePage();
    void UpdateVotePage();
    void ClearPage();
    void BindVote(EventSignal<WMouseEvent>& signal, const uint32& id);

    void Vote(const uint32& id);

    /// stores vote informations
    std::vector<VoteInfo> votesInfo;
};

#endif //VOTE_H_INCLUDED
