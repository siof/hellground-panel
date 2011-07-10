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

#include "textItems.h"

TextItems::TextItems(SessionInfo * sess, uint32 txtId)
{
    if (!sess || !txtId)
        return;

    label = new WText(sess->GetText(txtId));
    textId = txtId;
}

TextItems::TextItems(WString &lbl)
{
    label = new WText(lbl);
    textId = 0;
}

TextItems::~TextItems()
{
    if (label)
        delete label;

    label = NULL;
}

void TextItems::UpdateLabel(SessionInfo * sess, uint32 txtId)
{
    SetLabel(sess, txtId);
}

void TextItems::UpdateText(SessionInfo * sess, uint32 txtId)
{
    SetLabel(sess, txtId);
}

void TextItems::SetLabel(WText * lbl, uint32 id)
{
    if (!lbl)
        return;

    if (id)
        textId = id;

    if (label)
        delete label;

    label = lbl;
    lbl = NULL;
}

void TextItems::SetLabel(WString &lbl)
{
    if (!label)
        label = new WText(lbl);
    else
        label->setText(lbl);
}

void TextItems::SetLabel(SessionInfo * sess, uint32 id)
{
    if (!sess)
        return;

    if (id)
        textId = id;

    if (!textId)
        return;

    SetLabel(sess->GetText(textId));
}

void TextItems::SetText(WText * txt, uint32 id)
{
    SetLabel(txt, id);
}

void TextItems::SetText(WString &txt)
{
    SetLabel(txt);
}

void TextItems::SetText(SessionInfo * sess, uint32 id)
{
    SetLabel(sess, id);
}
