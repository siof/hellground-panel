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
 * \addtogroup Logout Logout page
 * \{
 *
 * \file logout.cpp
 * This file contains code needed to show and handle logout page.
 *
 ***********************************************/

#include "logout.h"

#include <Wt/WBreak>
#include <Wt/WPushButton>
#include <Wt/WTemplate>
#include <Wt/WText>

#include "../misc.h"

/********************************************//**
 * \brief Creates new LogoutPage object.
 *
 * \param sess      Contains current session informations.
 * \param parent    Parent container type object in which this container should be placed.
 *
 ***********************************************/

LogoutPage::LogoutPage(SessionInfo * session, WTemplate * tmpl, WContainerWidget * parent)
: WContainerWidget(parent), sess(session), templ(tmpl)
{
    btn = NULL;

    CreateLogoutPage();
    setStyleClass("page logoutwidget");
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
    Misc::Console(DEBUG_CODE, "\nLogoutPage::refresh()\n");
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
    Misc::Console(DEBUG_CODE, "\nLogoutPage::CreateLogoutPage()\n");

    btn = new WPushButton(Wt::WString::tr(TXT_BTN_LOGOUT));
    btn->clicked().connect(this, &LogoutPage::Logout);

    addWidget(new WText(Wt::WString::tr(TXT_INFO_LOGOUT)));
    addWidget(new WBreak());
    addWidget(new WBreak());
    addWidget(new WBreak());
    addWidget(new WBreak());
    addWidget(btn);
}

/********************************************//**
 * \brief Logouts player
 *
 * This function logouts current player and reset session value.
 *
 ***********************************************/

void LogoutPage::Logout()
{
    Misc::Console(DEBUG_CODE, "\nLogoutPage::Logout()\n");

    sess->Clear();

    templ->setCondition("if-loggedin", false);
    templ->setCondition("if-notlogged", true);
    templ->refresh();
}
