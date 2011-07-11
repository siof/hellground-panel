/*
*    HG Players Panel - web panel for HellGround server Players
*    Copyright (C) 2011 HellGround Team : Siof, lukaasm,
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU Affero General Public License as
*    published by the Free Software Foundation, either version 3 of the
*    License, or (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Affero General Public License for more details.
*
*    You should have received a copy of the GNU Affero General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ACCINFO_H_INCLUDED
#define ACCINFO_H_INCLUDED

#include "../defines.h"

enum AccountInfoSlot    // also determines items order
{
    ACCINFO_SLOT_INFO   = 0,
    ACCINFO_SLOT_TYPE,
    ACCINFO_SLOT_CURRENT_IP,
    ACCINFO_SLOT_CREATE_DATE,
    ACCINFO_SLOT_LAST_LOGIN_DATE,
    ACCINFO_SLOT_LAST_LOGGED_IP,
    ACCINFO_SLOT_IP_LOCK,
    ACCINFO_SLOT_ONLINE,
    ACCINFO_SLOT_CLIENT_VERSION,
    ACCINFO_SLOT_VOTE_POINTS,
    ACCINFO_SLOT_MULTIACC,
    ACCINFO_SLOT_ACC_BAN,
    ACCINFO_SLOT_LAST_IP_BAN,
    ACCINFO_SLOT_CURR_IP_BAN,

    ACCINFO_SLOT_COUNT
};

class AccountInfoSlotItem
{
public:
    AccountInfoSlotItem() : textId(0), breaks(0), label(NULL), widget(NULL) {}
    AccountInfoSlotItem(WText * lbl, WWidget * wid) : textId(0), breaks(0), label(lbl), widget(wid) {}
    ~AccountInfoSlotItem();

    void UpdateLabel(SessionInfo * sess);

    void SetLabel(WText * lbl);
    void SetWidget(WWidget * wid);
    void SetBreakCount(int br) { breaks = br; }
    void SetTextId(uint32 txtId) { textId = txtId; }

    void SetAll(WText * txt, WWidget * wid, int br, uint32 txtId);

    WText * GetLabel() { return label; }
    WWidget * GetWidget() { return widget; }
    int GetBreakCount() { return breaks; }
private:
    uint32 textId;
    int breaks;
    WText * label;
    WWidget * widget;
};

class AccountInfoPage : public WContainerWidget
{
public:
    AccountInfoPage(SessionInfo * sess, WContainerWidget * parent = 0);
    ~AccountInfoPage();

    void refresh();     // overload WWidget::refresh() for automatic content change ;) this should be done for all pages
private:
    SessionInfo * session;
    bool needInfoCreation;

    AccountInfoSlotItem accInfoSlots[ACCINFO_SLOT_COUNT];

    void UpdateTextWidgets();
    void CreateAccountInfo();
    void UpdateAccountInfo();
};

#endif // ACCINFO_H_INCLUDED
