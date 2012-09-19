/*
*    HG Players Panel - web panel for HellGround server Players
*    Copyright (C) 2011-2012 HellGround Team : Siof, lukaasm,
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
 * \addtogroup Support Support project
 * \{
 *
 * \file support.cpp
 * This file contains code needed to provide support possibility.
 *
 ***********************************************/

#include "support.h"

#include <Wt/WText>

/********************************************//**
 * \brief Creates new SupportPage object.
 *
 * \param sess      Contains user session informations.
 * \param parent    Parent container widget to which should be added this widget.
 *
 ***********************************************/

SupportPage::SupportPage(SessionInfo * sess, WContainerWidget * parent):
    WContainerWidget(parent), session(sess)
{
    needCreation = true;

    setStyleClass("page supportwidget");
}

SupportPage::~SupportPage()
{
    session = NULL;
}

/********************************************//**
 * \brief Overloads WContainerWidget::refresh() for automatic content change.
 *
 * This function can create new or update existing content.
 * In most situations this is used for content update ;)
 *
 ***********************************************/

void SupportPage::refresh()
{
    if (isHidden() || isDisabled())
        return;

    // only logged in players can visit this page so there is no need to create/update it in other cases
    if (session->accLvl > LVL_NOT_LOGGED)
    {
        if (needCreation)
        {
            needCreation = false;
            addWidget(new WText(tr(TXT_INFO_SUPPORT)));
        }
    }
    else
    {
        clear();
        needCreation = true;
    }

    WContainerWidget::refresh();
}
