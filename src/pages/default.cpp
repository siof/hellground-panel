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
 * \addtogroup Default Default page
 * \{
 *
 * \file default.cpp
 * This file contains code needed to show default (home/start) page.
 *
 ***********************************************/

#include "default.h"

#include <Wt/WText>

/********************************************//**
 * \brief Creates new DefaultPage object.
 *
 * \param sess      Contains current session informations.
 * \param parent    Parent container type object in which this container should be placed.
 *
 ***********************************************/

DefaultPage::DefaultPage(WContainerWidget * parent)
: WContainerWidget(parent)
{
    new WText(Wt::WString::tr(TXT_INFO_SERVER), this);
}

DefaultPage::~DefaultPage()
{

}

/********************************************//**
 * \}
 * \}
 ***********************************************/
