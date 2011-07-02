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

#include "error.h"


ErrorPageSlot::~ErrorPageSlot()
{
    if (text)
        delete text;

    text = NULL;
}

void ErrorPageSlot::SetText(WText * txt, uint32 id)
{
    DeleteText();

    text = txt;
    textId = id;
    textIdBased = true;
}

void ErrorPageSlot::SetText(WText * txt, WString &strn)
{
    DeleteText();

    text = txt;

    str = strn;
    textIdBased = false;
}

void ErrorPageSlot::SetText(WString &strn)
{
    str = strn;
    textIdBased = false;
}

void ErrorPageSlot::SetText(SessionInfo * sess, uint32 id)
{
    #ifdef DEBUG
    printf("\nErrorPageSlot::SetText(sess: %i)\n", sess ? 1 : 0);
    #endif
    if (!sess)
        return;

    textId = id;
    textIdBased = true;
}

WText * ErrorPageSlot::CreateText(SessionInfo * sess)
{
    #ifdef DEBUG
    printf("\nErrorPageSlot::CreateText(sess: %i)\n", sess ? 1 : 0);
    #endif
    if (!sess)
        return NULL;

    if (text)
    {
        UpdateText(sess);
        return text;
    }

    if (textIdBased)
    {
        if (textId)
            text = new WText(sess->GetText(textId));
    }
    else if (!str.empty())
        text = new WText(str);

    return text;
}

void ErrorPageSlot::DeleteText()
{
    if (text)
        delete text;

    text = NULL;
}

void ErrorPageSlot::UpdateText(SessionInfo * sess)
{
    #ifdef DEBUG
    printf("\nErrorPageSlot::UpdateText(sess: %i)\n", sess ? 1 : 0);
    #endif
    if (!sess || !text)
        return;

    if (textIdBased && textId)
        text->setText(sess->GetText(textId));
    else
        text->setText(str);
}


ErrorPage::ErrorPage(SessionInfo * sess, WContainerWidget * parent)
: WContainerWidget(parent), session(sess), created(false)
{
    #ifdef DEBUG
    printf("\nErrorPage::ErrorPage(sess: %i)\n", sess ? 1 : 0);
    #endif
}

void ErrorPage::refresh()
{
    #ifdef DEBUG
    printf("\nErrorPage::refresh()\n");
    #endif

    if (created)
        UpdateErrors();
    else
        CreateErrors();

    WContainerWidget::refresh();
}

void ErrorPage::SetErrorMsg(ErrorSlots slot, uint32 txtId)
{
    #ifndef SHOW_DATABASE_ERRORS
    if (slot == ERROR_SLOT_DB)
        return;
    #endif

    errors[slot].SetText(session, txtId);
}

void ErrorPage::SetErrorMsg(ErrorSlots slot, WString &str)
{
    #ifndef SHOW_DATABASE_ERRORS
    if (slot == ERROR_SLOT_DB)
        return;
    #endif

    errors[slot].SetText(str);
}

void ErrorPage::UpdateErrors()
{
    for (int i = 0; i < ERROR_SLOT_COUNT; ++i)
        #ifndef SHOW_DATABASE_ERRORS
        if (i != ERROR_SLOT_DB)
        #endif
            errors[i].UpdateText(session);
}

void ErrorPage::CreateErrors()
{
    WText * tmpW = NULL;
    for (int i = 0; i < ERROR_SLOT_COUNT; ++i)
    {
        #ifndef SHOW_DATABASE_ERRORS
        if (i != ERROR_SLOT_DB)
        {
        #endif
            if (tmpW = errors[i].CreateText(session))
            {
                addWidget(tmpW);
                addWidget(new WBreak());
                addWidget(new WBreak());
            }
        #ifndef SHOW_DATABASE_ERRORS
        }
        #endif
    }
}
