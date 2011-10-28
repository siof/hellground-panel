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
    console(DEBUG_CODE, "BasicTextItem::BasicTextItem(SessionInfo * sess = %i, uint32 txtId = %u)\n", sess != NULL, txtId);
    if (!sess || !txtId)
        return;

    label = new WText(sess->GetText(txtId));
    textId = txtId;
    prevLang = sess->language;
}

BasicTextItem::BasicTextItem(WString &lbl)
{
    console(DEBUG_CODE, "BasicTextItem::BasicTextItem(WString &lbl = %s)\n", lbl.toUTF8().c_str());
    label = new WText(lbl);
    textId = 0;
    prevLang = LANG_COUNT;
}

BasicTextItem::~BasicTextItem()
{
//    if (label)
//        delete label;

//    label = NULL;
}

void BasicTextItem::UpdateLabel(SessionInfo * sess, uint32 txtId)
{
    console(DEBUG_CODE, "void BasicTextItem::UpdateLabel(SessionInfo * sess = %i, uint32 txtId = %u)\n", sess != NULL, txtId);
    if (prevLang != sess->language)
        SetLabel(sess, txtId);

    prevLang = sess->language;
}

void BasicTextItem::UpdateText(SessionInfo * sess, uint32 txtId)
{
    console(DEBUG_CODE, "void BasicTextItem::UpdateText(SessionInfo * sess = %i, uint32 txtId = %u)\n", sess != NULL, txtId);
    if (prevLang != sess->language)
        SetLabel(sess, txtId);

    prevLang = sess->language;
}

void BasicTextItem::SetLabel(WText * lbl, uint32 id)
{
    console(DEBUG_CODE, "void BasicTextItem::SetLabel(WText * lbl = %i, uint32 id = %u)\n", lbl != NULL, id);
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
    console(DEBUG_CODE, "void BasicTextItem::SetLabel(WString &lbl = %s)\n", lbl.toUTF8().c_str());
    if (!label)
        label = new WText(lbl);
    else
        label->setText(lbl);
}

void BasicTextItem::SetLabel(SessionInfo * sess, uint32 id)
{
    console(DEBUG_CODE, "void BasicTextItem::SetLabel(SessionInfo * sess = %i, uint32 id = %u) prevLang = %i, lang = %i\n", sess != NULL, id, prevLang, sess->language);
    if (!sess || prevLang == sess->language)
        return;

    if (id)
        textId = id;

    if (!textId)
        return;

    SetLabel(sess->GetText(textId));
    prevLang = sess->language;
}


void BasicTextItem::SetLabel(std::string &lbl)
{
    console(DEBUG_CODE, "void BasicTextItem::SetLabel(std::string &lbl = %s)\n", lbl.c_str());
    WString tmpStr = WString::fromUTF8(lbl);
    SetLabel(tmpStr);
}

void BasicTextItem::SetLabel(const char* lbl)
{
    console(DEBUG_CODE, "void BasicTextItem::SetLabel(const char* lbl)\n", lbl);
    WString tmpStr = WString::fromUTF8(lbl);
    SetLabel(tmpStr);
}

void BasicTextItem::SetText(WText * txt, uint32 id)
{
    console(DEBUG_CODE, "void BasicTextItem::SetText(WText * txt = %i, uint32 id = %u)\n", txt != NULL, id);
    SetLabel(txt, id);
}

void BasicTextItem::SetText(WString &txt)
{
    console(DEBUG_CODE, "void BasicTextItem::SetText(WString &txt = %s)\n", txt.toUTF8().c_str());
    SetLabel(txt);
}

void BasicTextItem::SetText(SessionInfo * sess, uint32 id)
{
    console(DEBUG_CODE, "void BasicTextItem::SetText(SessionInfo * sess = %i, uint32 id = %u)\n", sess != NULL, id);
    SetLabel(sess, id);
}

void BasicTextItem::SetText(std::string &lbl)
{
    console(DEBUG_CODE, "void BasicTextItem::SetText(std::string &lbl = %s)\n", lbl.c_str());
    SetLabel(lbl);
}

void BasicTextItem::SetText(const char* lbl)
{
    console(DEBUG_CODE, "void BasicTextItem::SetText(const char* lbl = %s)\n", lbl);
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
    console(DEBUG_CODE, "void PageSlotItem::SetLabel(WText * lbl = %i)\n", lbl != NULL);
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
    console(DEBUG_CODE, "void PageSlotItem::SetLabel(SessionInfo * sess = %i, uint32 txtId = %u)\n", sess != NULL, txtId);
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
    console(DEBUG_CODE, "void PageSlotItem::SetWidget(WWidget * wid = %i)\n", wid != NULL);
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
    console(DEBUG_CODE, "void PageSlotItem::SetAll(WText * txt = %i, WWidget * wid = %i, int br = %i, uint32 txtId = %u)\n", txt != NULL, wid != NULL, br, txtId);
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
    console(DEBUG_CODE, "void PageSlotItem::SetAll(SessionInfo * sess = %i, uint32 txtId = %u, WWidget * wid = %i, int br = %i)\n", sess != NULL, txtId, wid != NULL, br);
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
    console(DEBUG_CODE, "void PageSlotItem::UpdateLabel(SessionInfo * sess = %i, uint32 txtId = %u)\n", sess != NULL, txtId);
    label.UpdateLabel(sess, txtId);
}
