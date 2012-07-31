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

#include "main.h"

#include <Wt/WEnvironment>
#include <Wt/WImage>
#include <Wt/WMenu>
#include <Wt/WMenuItem>
#include <Wt/WOverlayLoadingIndicator>
#include <Wt/WPushButton>
#include <Wt/WStackedWidget>
#include <Wt/WTable>
#include <Wt/WText>
#include <Wt/WTemplate>

#include "defines.h"
#include "database.h"
#include "menu.h"
#include "misc.h"
#include "LangsWidget.h"
#include "login.h"

PlayersPanel::PlayersPanel(const Wt::WEnvironment& env)
    : WApplication(env)
{
    session = new SessionInfo();
    session->sessionIp = env.clientAddress();

    setLoadingIndicator(new Wt::WOverlayLoadingIndicator("loading"));

    setTitle(Wt::WString::tr(TXT_SITE_TITLE));

    useStyleSheet("res/style.css");
    root()->setStyleClass("main");

    messageResourceBundle().use("langs/panel");

    // page creation
    templ = new Wt::WTemplate();

    content =  new Wt::WStackedWidget();
    langs = new LangsWidget();
    menu = CreateMenu();
    login = new LoginWidget(session, templ);

    templ->bindWidget("add-langs", langs);
    templ->bindWidget("add-login", login);
    templ->bindWidget("add-menu", menu);
    templ->bindWidget("add-content", content);
    templ->bindWidget("add-profile", new Wt::WText("testus profile"));

    templ->addFunction("tr", &Wt::WTemplate::Functions::tr);
    templ->setCondition("if-loggedin", false);
    templ->setCondition("if-notlogged", true);

    templ->setTemplateText(
        "<div class=\"head\" ></div>"
        "<div class=\"sidebar\" >"

            "<div class=\"profile-top\"></div>"
            "<div class=\"profile\" >"
//                "${<if-loggedin>}"
//                    "${add-profile}"
//                "${</if-loggedin>}"
                "${<if-notlogged>}"
                    "${add-login}"
                "${</if-notlogged>}"
            "</div>"
            "<div class=\"profile-bottom\"></div>"

            "<div class=\"langs-top\"></div>"
            "<div class=\"langs\" >${add-langs}</div>"
            "<div class=\"langs-bottom\"></div>"

            "<div class=\"menu-top\"></div>"
            "<div class=\"menu\">${add-menu}</div>"
            "<div class=\"menu-bottom\"></div>"

        "</div>"
        "<div class=\"content\" >${add-content}</div> "
        "<div class=\"footer\" >${tr:site.footer}</div>");

    root()->addWidget(templ);
}

Wt::WMenu * PlayersPanel::CreateMenu(Wt::Orientation ori)
{
    menu = new HGMenu(content, session, templ, ori);

    return menu;
}

PlayersPanel::~PlayersPanel()
{
    delete content;
    delete session;
}

WApplication * CreateApplication(const Wt::WEnvironment& env)
{
    Misc::Console(DEBUG_CODE, "Call WApplication * CreateApplication(const WEnvironment& env)");
    // You could read information from the environment to decide
    // whether the user has permission to start a new application

    PlayersPanel * tmpPanel = new PlayersPanel(env);

    return tmpPanel;
}

int main(int argc, char **argv)
{
    srand(time(NULL));
    return WRun(argc, argv, &CreateApplication);
}
