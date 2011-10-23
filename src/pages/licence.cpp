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

LicencePage::LicencePage(SessionInfo * sess, WContainerWidget * parent)
: WContainerWidget(parent)
{
    setContentAlignment(AlignCenter|AlignTop);
    needCreation = true;
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

void LicencePage::refresh()
{
    if (isHidden() || isDisabled())
        return;

    if (needCreation)
        ShowText();
    else
        UpdateText();

    WContainerWidget::refresh();
}

/********************************************//**
 * \brief Shows content of licence page
 *
 * This function deletes old and creates new content depends on actual language.
 *
 ***********************************************/

void LicencePage::ShowText()
{
    needCreation = false;

    licenceTextSlots[LICENCE_SLOT_INTRO].SetLabel(session, TXT_LICENCE_INTRO);
    licenceTextSlots[LICENCE_SLOT_REPO].SetLabel(session, TXT_LICENCE_REPO);
    licenceTextSlots[LICENCE_SLOT_INFO].SetLabel(session, TXT_LICENCE_INFO);

    addWidget(licenceTextSlots[LICENCE_SLOT_INTRO].GetText());
    addWidget(new WBreak());
    addWidget(new WBreak());
    addWidget(licenceTextSlots[LICENCE_SLOT_REPO].GetText());
    addWidget(new WBreak());
    addWidget(new WBreak());
    addWidget(new WBreak());
    addWidget(licenceTextSlots[LICENCE_SLOT_INFO].GetText());
}

/********************************************//**
 * \brief Updates content of licence page
 *
 * This function updates content depends on actual language.
 *
 ***********************************************/

void LicencePage::UpdateText()
{
    for (int i = 0; i < LICENCE_SLOT_COUNT; ++i)
        licenceTextSlots[i].UpdateText(session);
}
