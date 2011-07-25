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

#ifndef TEXTITEMS_H_INCLUDED
#define TEXTITEMS_H_INCLUDED

#include "defines.h"

class TextItems
{
public:
    TextItems() : textId(0), label(NULL) {}
    TextItems(SessionInfo * sess, uint32 txtId);
    TextItems(WString &lbl);
    ~TextItems();

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
private:
    uint32 textId;
    WText * label;
};

#endif // TEXTITEMS_H_INCLUDED
