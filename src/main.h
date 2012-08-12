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

#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <Wt/WApplication>
#include <Wt/WEnvironment>

#include "defines.h"

class LangsWidget;
class LoginWidget;
class HGMenu;
class TemplateWidget;

class PlayersPanel : public Wt::WApplication
{
public:
    PlayersPanel(const Wt::WEnvironment& env);
    ~PlayersPanel();

private:
    Wt::WStackedWidget * content;       // container to show menu items after click (main content container)
    SessionInfo * session;              // store info about user session

    Wt::WTemplate * templ;
    LangsWidget * langs;
    LoginWidget * login;
    HGMenu * menu;
    TemplateWidget * templChooser;

    HGMenu * CreateMenu(Wt::Orientation ori = Wt::Horizontal);
};

#endif // MAIN_H_INCLUDED
