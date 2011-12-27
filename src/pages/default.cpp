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
 * \addtogroup Default Default page
 * \{
 *
 * \file default.cpp
 * This file contains code needed to show default (home/start) page.
 *
 ***********************************************/

#include "default.h"

/********************************************//**
 * \brief Creates new DefaultPage object.
 *
 * \param sess      Contains current session informations.
 * \param parent    Parent container type object in which this container should be placed.
 *
 ***********************************************/

DefaultPage::DefaultPage(SessionInfo * sess, WContainerWidget * parent)
: WContainerWidget(parent)
{
    session = sess;
    ShowText();
}

/********************************************//**
 * \brief Overloads WContainerWidget::refresh() for automatic content change.
 *
 * This function can delete old and create new content.
 * In most situations this is used for content update or language change ;)
 *
 ***********************************************/

void DefaultPage::refresh()
{
    if (isHidden() || isDisabled())
        return;

    ShowText();

    WContainerWidget::refresh();
}

/********************************************//**
 * \brief Shows content of default page
 *
 * This function deletes old and creates new content depends on actual language.
 *
 ***********************************************/

void DefaultPage::ShowText()
{
    clear();

    new WText(session->GetText(TXT_SERVER_INFO), this);
}
