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

#include "slotItems.h"

BasicTextItem::BasicTextItem(SessionInfo * sess, uint32 txtId)
{
    if (!sess || !txtId)
        return;

    label = new WText(sess->GetText(txtId));
    textId = txtId;
}

BasicTextItem::BasicTextItem(WString &lbl)
{
    label = new WText(lbl);
    textId = 0;
}

BasicTextItem::~BasicTextItem()
{
//    if (label)
//        delete label;

//    label = NULL;
}

void BasicTextItem::UpdateLabel(SessionInfo * sess, uint32 txtId)
{
    SetLabel(sess, txtId);
}

void BasicTextItem::UpdateText(SessionInfo * sess, uint32 txtId)
{
    SetLabel(sess, txtId);
}

void BasicTextItem::SetLabel(WText * lbl, uint32 id)
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

void BasicTextItem::SetLabel(WString &lbl)
{
    if (!label)
        label = new WText(lbl);
    else
        label->setText(lbl);
}

void BasicTextItem::SetLabel(SessionInfo * sess, uint32 id)
{
    if (!sess)
        return;

    if (id)
        textId = id;

    if (!textId)
        return;

    SetLabel(sess->GetText(textId));
}


void BasicTextItem::SetLabel(std::string &lbl)
{
    WString tmpStr = WString::fromUTF8(lbl);
    SetLabel(tmpStr);
}

void BasicTextItem::SetLabel(const char* lbl)
{
    WString tmpStr = WString::fromUTF8(lbl);
    SetLabel(tmpStr);
}

void BasicTextItem::SetText(WText * txt, uint32 id)
{
    SetLabel(txt, id);
}

void BasicTextItem::SetText(WString &txt)
{
    SetLabel(txt);
}

void BasicTextItem::SetText(SessionInfo * sess, uint32 id)
{
    SetLabel(sess, id);
}


void BasicTextItem::SetText(std::string &lbl)
{
    SetLabel(lbl);
}

void BasicTextItem::SetText(const char* lbl)
{
    SetLabel(lbl);
}


PageSlotItem::~PageSlotItem()
{

}

/********************************************//**
 * \brief Sets new text label widget (and deletes old one).
 *
 * \param lbl   Text label widget.
 *
 ***********************************************/

void PageSlotItem::SetLabel(WText * lbl)
{
    label.SetLabel(lbl);
}

/********************************************//**
 * \brief Sets new text label widget (and deletes old one).
 *
 * \param sess   Current session informations.
 * \param txtId  Text id from DB which should be shown.
 *
 ***********************************************/

void PageSlotItem::SetLabel(SessionInfo * sess, uint32 txtId)
{
    label.SetLabel(sess, txtId);
}

/********************************************//**
 * \brief Sets new informations widget (and deletes old one).
 *
 * \param wid   Widget with informations.
 *
 ***********************************************/

void PageSlotItem::SetWidget(WWidget * wid)
{
    if (widget)
        delete widget;

    widget = wid;
}

/********************************************//**
 * \brief Sets all informations in one time.
 *
 * \param txt   Text label widget.
 * \param wid   Widget with informations.
 * \param br    Count of breaks/new lines after this item.
 * \param txtId Text id for text label widget. Optional.
 *
 ***********************************************/

void PageSlotItem::SetAll(WText * txt, WWidget * wid, int br, uint32 txtId)
{
    SetLabel(txt);
    SetWidget(wid);
    SetBreakCount(br);
    SetTextId(txtId);
}

/********************************************//**
 * \brief Sets all informations in one time.
 *
 * \param sess  User session informations.
 * \param txtId Text id for text label widget.
 * \param wid   Widget with informations.
 * \param br    Count of breaks/new lines after this item.
 *
 ***********************************************/

void PageSlotItem::SetAll(SessionInfo * sess, uint32 txtId, WWidget * wid, int br)
{
    SetLabel(sess, txtId);
    SetWidget(wid);
    SetBreakCount(br);
}

/********************************************//**
 * \brief Updates text label widget depends on language.
 *
 * \param sess  Current session informations.
 * \param txtId New label text id. Optional.
 *
 ***********************************************/

void PageSlotItem::UpdateLabel(SessionInfo * sess, uint32 txtId)
{
    label.UpdateLabel(sess, txtId);
}
