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
#include "TemplateWidget.h"

PlayersPanel::PlayersPanel(const Wt::WEnvironment& env)
    : WApplication(env)
{
    session = new SessionInfo();
    session->sessionIp = env.clientAddress();

    setLoadingIndicator(new Wt::WOverlayLoadingIndicator());
    loadingIndicator()->setMessage(Wt::WString::tr(TXT_GEN_LOADING));
    messageResourceBundle().use("langs/panel");

    setTitle(Wt::WString::tr(TXT_SITE_TITLE));

    // template info - assign default paths for default template
    TemplateInfo tmplt;

    const std::string * cookieVal = env.getCookieValue("tmplt");
    if (cookieVal && !cookieVal->empty() && *cookieVal != "0")
        tmplt = Misc::GetTemplateInfoFromDB(*cookieVal);
    else
    {
        tmplt.name = TMPLT_DEFAULT_NAME;
        tmplt.stylePath = TMPLT_DEFAULT_STYLE_PATH;
        tmplt.tmpltPath = TMPLT_DEFAULT_TMPLT_PATH;
        tmplt.currentTemplate = Misc::GetTemplate(tmplt.tmpltPath, tmplt.name);
    }

    useStyleSheet(tmplt.GetFullStylePath());

    root()->setStyleClass("main");

    // page creation
    templ = new Wt::WTemplate(root());

    content =  new Wt::WStackedWidget();
    langs = new LangsWidget();
    login = new LoginWidget(session, templ);
    menu = new HGMenu(content, session, templ);
    templChooser = new TemplateWidget(templ);

    templ->bindWidget("add-langs", langs);
    templ->bindWidget("add-login", login);
    templ->bindWidget("add-menu", menu);
    templ->bindWidget("add-content", content);
    templ->bindWidget("add-profile", new Wt::WText("testus profile"));
    templ->bindWidget("add-templatechooser", templChooser);
    templ->bindWidget("add-footer", new Wt::WText(Wt::WString::tr(TXT_SITE_FOOTER)));

    templ->addFunction("tr", &Wt::WTemplate::Functions::tr);
    templ->setCondition("if-loggedin", false);
    templ->setCondition("if-notlogged", true);

    templ->setTemplateText(tmplt.currentTemplate);
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
