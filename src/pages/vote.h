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
 * This file contains th headers needed to provide vote possibility.
 *
 ***********************************************/

#ifndef VOTE_H_INCLUDED
#define VOTE_H_INCLUDED

#include "../defines.h"
#include "../slotItems.h"

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
 * \brief Class to represent Vote Page Slot Item.
 ***********************************************/

class VoteSlotItem : public PageSlotItem
{
public:
    VoteSlotItem() : PageSlotItem(), disabled(false), voteId(0), expire(""), voteName("") {}
    ~VoteSlotItem(){}

    bool IsDisabled() { return disabled; }
    void SetDisabled(bool disabled) { this->disabled = disabled; }

    uint32 GetVoteId() { return voteId; }
    void SetVoteId(uint32 id) { voteId = id; }

    WString GetExpire() { return expire; }
    void SetExpire(const WString & exp) { expire = exp; }

    WString GetName() { return voteName; }
    void SetName(const WString & name) { voteName = name; }

    void UpdateExpireLabel(SessionInfo * sess)
    {
        if (disabled)
        {
            WString tmpLbl = voteName + sess->GetText(TXT_SUPPORT_VOTE_NEXT) + expire;
            SetLabel(tmpLbl);
        }
    }

private:
    bool disabled;
    uint32 voteId;
    WString expire;
    WString voteName;
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
    /// created or not?
    bool needCreation;
    /// vote info slots
    BasicTextItem infoSlots[VOTE_SLOT_COUNT];

    void CreateVotePage();
    void UpdateVotePage();
    void ClearPage();
    void BindVote(EventSignal<WMouseEvent>& signal, const uint32& id);

    void Vote(const uint32& id);

    /// stores vote state and vote 'slot' informations
    std::map<uint32, VoteSlotItem> voteMap;
};

#endif //VOTE_H_INCLUDED
