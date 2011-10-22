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

#ifndef SLOTITEMS_H_INCLUDED
#define SLOTITEMS_H_INCLUDED

#include "defines.h"

/********************************************//**
 * \brief Represents basic text labels.
 *
 * Each item can contain text label which should be displayed on page.
 * Class can also update text labels depends on actual language but
 * this needs support from db and session informations.
 *
 ***********************************************/

class BasicTextItem
{
public:
    BasicTextItem() : textId(0), label(NULL) {}
    BasicTextItem(SessionInfo * sess, uint32 txtId);
    BasicTextItem(WString &lbl);
    ~BasicTextItem();

    void UpdateLabel(SessionInfo * sess, uint32 txtId = 0);
    void UpdateText(SessionInfo * sess, uint32 txtId = 0);

    void SetLabel(WText * lbl, uint32 id = 0);
    void SetLabel(WString &lbl);
    void SetLabel(SessionInfo * sess, uint32 id);
    void SetLabel(std::string &lbl);
    void SetLabel(const char* lbl);

    void SetText(WText * txt, uint32 id = 0);
    void SetText(WString &lbl);
    void SetText(SessionInfo * sess, uint32 id);
    void SetText(std::string &lbl);
    void SetText(const char* lbl);

    void SetTextId(uint32 txtId) { textId = txtId; }

    WText * GetLabel() { return label; }
    WText * GetText() { return label; }

    void Clear()
    {
        textId = 0;
        label = NULL;
    }

private:
    uint32 textId;
    WText * label;
};

/********************************************//**
 * \brief Contains text label and single information widget.
 *
 * Each slot item can contain text label and widget which should be displayed on page.
 * Class can also update text labels depends on actual language but
 * this needs support from db and session informations.
 * Break count tells how much new lines should be added after information widget.
 * Text label and information widget are in the same line (no new lines beetwean them).
 *
 ***********************************************/

class PageSlotItem
{
public:
    PageSlotItem() : breaks(0), widget(NULL) {}
    ~PageSlotItem();

    void UpdateLabel(SessionInfo * sess, uint32 txtId = 0);

    void SetLabel(WText * lbl);
    void SetLabel(SessionInfo * sess, uint32 txtId);
    void SetWidget(WWidget * wid);
    void SetBreakCount(int br) { breaks = br; }
    void SetTextId(uint32 txtId) { label.SetTextId(txtId); }

    void SetAll(WText * txt, WWidget * wid, int br, uint32 txtId = 0);
    void SetAll(SessionInfo * sess, uint32 txtId, WWidget * wid, int br);

    WText * GetLabel() { return label.GetLabel(); }
    WWidget * GetWidget() { return widget; }
    int GetBreakCount() { return breaks; }

    void Clear()
    {
        breaks = 0;
        label.Clear();
        widget = NULL;
    }

private:
    int breaks;
    BasicTextItem label;
    WWidget * widget;
};

#endif // SessionInfo
