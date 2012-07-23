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

#include <Wt/WContainerWidget>

#include "defines.h"

class LoginWidget : public Wt::WContainerWidget
{
public:
    LoginWidget(SessionInfo * sess, Wt::WTemplate * tmplt, Wt::WContainerWidget * parent = NULL);
    ~LoginWidget();

private:
    SessionInfo * session;

    Wt::WTemplate * templ;
    Wt::WLineEdit * login;
    Wt::WLineEdit * pass;
    Wt::WPushButton * btn;

    void Login();
    void ShowError(const char * name, const char * txt);
    void AddActivityLogIn(bool success, const char * login = NULL);
    void AddActivityLogIn(uint32 id, bool success);
};
