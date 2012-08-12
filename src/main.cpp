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

#include <fstream>

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
    messageResourceBundle().use("langs/panel");

    setTitle(Wt::WString::tr(TXT_SITE_TITLE));

    // template info - assign default paths for default template
    TemplateInfo tmplt(TMPLT_DEFAULT_NAME, TMPLT_DEFAULT_STYLE_PATH, TMPLT_DEFAULT_TMPLT_PATH);

    const std::string * cookieVal = env.getCookieValue("tmplt");
    if (cookieVal && !cookieVal->empty() && *cookieVal != "0")
    {
        Database db;
        if (db.Connect(PANEL_DB_DATA, SQL_PANELDB))
        {
            std::string tmpName = db.EscapeString(*cookieVal);
            if (db.ExecutePQuery("SELECT name, stylePath, tmpltPath FROM Templates WHERE name = '%s'", tmpName.c_str()) > DB_RESULT_EMPTY)
            {
                tmplt.name = db.GetRow()->fields[0].GetString();
                tmplt.stylePath = db.GetRow()->fields[1].GetString();
                tmplt.tmpltPath = db.GetRow()->fields[2].GetString();
            }
        }
    }

    useStyleSheet(tmplt.GetFullStylePath());

    root()->setStyleClass("main");

    std::fstream tmpltFile;

    tmpltFile.open(tmplt.GetFullTemplatePath().c_str(), std::fstream::in);
    if (tmpltFile.is_open() && tmpltFile.good())
    {
        char tmpStr[1000];
        while (!tmpltFile.eof())
        {
            tmpltFile.getline(tmpStr, 1000, '\n');
            tmplt.currentTemplate += tmpStr;
        }
    }
    tmpltFile.close();

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

    templ->setTemplateText(tmplt.currentTemplate);

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
