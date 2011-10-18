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
 * \addtogroup Logout Logout page
 * \{
 *
 * \file logout.cpp
 * This file contains code needed to show and handle logout page.
 *
 ***********************************************/

#include "logout.h"

/********************************************//**
 * \brief Creates new LogoutPage object.
 *
 * \param sess      Contains current session informations.
 * \param parent    Parent container type object in which this container should be placed.
 *
 ***********************************************/

LogoutPage::LogoutPage(SessionInfo * sess, WContainerWidget * parent)
: WContainerWidget(parent)
{
    session = sess;
    needCreation = true;
    info = NULL;
    btn = NULL;
}

/********************************************//**
 * \brief Overloads WContainerWidget::refresh() for automatic content change.
 *
 * This function can delete old and create new content.
 * In most situations this is used for content update or language change ;)
 *
 ***********************************************/

void LogoutPage::refresh()
{
    console(DEBUG_CODE, "\nLogoutPage::refresh()\n");

    if (isHidden() || isDisabled())
        return;

    if (session->accLvl > LVL_NOT_LOGGED)
    {
        if (needCreation)
            CreateLogoutPage();
        else
            UpdateLogoutPage();
    }

    WContainerWidget::refresh();
}

/********************************************//**
 * \brief Creates content of logout page
 *
 * This function deletes old and creates new content depends on actual language.
 *
 ***********************************************/

void LogoutPage::CreateLogoutPage()
{
    console(DEBUG_CODE, "\nLogoutPage::CreateLogoutPage()\n");

    clear();

    info = new WText(session->GetText(TXT_LOGOUT_INFO));
    btn = new WPushButton(session->GetText(TXT_BTN_LOGOUT));
    btn->clicked().connect(this, &LogoutPage::Logout);

    addWidget(new WBreak());
    addWidget(new WBreak());
    addWidget(info);
    addWidget(new WBreak());
    addWidget(new WBreak());
    addWidget(new WBreak());
    addWidget(new WBreak());
    addWidget(btn);
}

/********************************************//**
 * \brief Update content of logout page
 *
 * This function set new texts for content depends on actual language.
 *
 ***********************************************/

void LogoutPage::UpdateLogoutPage()
{
    console(DEBUG_CODE, "\nLogoutPage::UpdateLogoutPage()\n");

    info->setText(session->GetText(TXT_LOGOUT_INFO));
    btn->setText(session->GetText(TXT_BTN_LOGOUT));
}

/********************************************//**
 * \brief Logouts player
 *
 * This function logouts current player and reset session value.
 *
 ***********************************************/

void LogoutPage::Logout()
{
    console(DEBUG_CODE, "\nLogoutPage::Logout()\n");

    session->Clear();

    wApp->root()->refresh();
}
