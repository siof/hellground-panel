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
 * \addtogroup Licence Licence and informations page
 * \{
 *
 * \file licence.cpp
 * This file contains code needed to show informations/licence page.
 *
 ***********************************************/

#include "licence.h"

/********************************************//**
 * \brief Creates new LicencePage object.
 *
 * \param sess      Contains current session informations.
 * \param parent    Parent container type object in which this container should be placed.
 *
 ***********************************************/

LicencePage::LicencePage(WContainerWidget * parent)
: Wt::WContainerWidget(parent)
{
    setContentAlignment(AlignCenter|AlignTop);
    ShowText();
}

/********************************************//**
 * \brief Overloads WContainerWidget::refresh() for automatic content change.
 *
 * This function can delete old and create new content.
 * In most situations this is used for content update or language change ;)
 *
 ***********************************************/

void LicencePage::refresh()
{
    if (isHidden() || isDisabled())
        return;

    Wt::WContainerWidget::refresh();
}

/********************************************//**
 * \brief Shows content of licence page
 *
 * This function deletes old and creates new content depends on actual language.
 *
 ***********************************************/

void LicencePage::ShowText()
{
    addWidget(new Wt::WText(tr(TXT_LICENCE_INTRO)));
    addWidget(new Wt::WBreak());
    addWidget(new Wt::WBreak());
    addWidget(new Wt::WText(tr(TXT_LICENCE_REPO)));
    addWidget(new Wt::WBreak());
    addWidget(new Wt::WBreak());
    addWidget(new Wt::WBreak());
    addWidget(new Wt::WText(tr(TXT_LICENCE_INFO)));
}
